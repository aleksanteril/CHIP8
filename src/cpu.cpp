#include "cpu.hpp"
#include <iomanip>
#include <iostream>

CPU::CPU(Bus& bus)
  : bus(bus)
{       
        constexpr uint8_t font_set[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

        uint16_t addr { 0x50 };
        for (auto byte : font_set) // Load font data to 0x50 - 0x9F
                bus.write(addr++, byte);
        std::srand(std::time({}));
}

void
CPU::cycle()
{
        uint16_t opcode { bus.read(pc) }; // MSB
        opcode <<= 8;
        opcode |= bus.read(pc + 1); // LSB

        pc += 2;
        execute_op(opcode);
}

void inline print_opcode(uint16_t opcode)
{
        std::cout << std::hex << std::setfill('0');
        std::cout << std::setw(4) << opcode << '\n';
}

void
CPU::draw_framebuf(uint8_t x, uint8_t y, uint8_t n)
{
        // coords wrap, as specified
        int x_coord  { reg[x] % 64 };
        int y_coord  { reg[y] % 32 };
        reg[0xF] = 0;

        uint16_t addr { index_reg };
        for (auto i {0}; i < n; ++i, ++addr) {
                uint8_t sprite_byte = bus.read(addr);

                for (auto j {0}; j < 8; ++j, sprite_byte <<= 1) {
                        if (sprite_byte & 0x80) {
                                int draw_y { y_coord + i };
                                int draw_x { x_coord + j };

                                if (draw_x > 63 || draw_y > 31)
                                        continue;

                                int pos = (64 * draw_y) + draw_x;
                                if (framebuf[pos]) // Set VF flag on collision
                                        reg[0xF] = 1;
                                framebuf[pos] ^= 1;
                        }
                }
        }

}

std::array<bool, 64 * 32>& 
CPU::framebuf_ref()
{
        return framebuf;
}

void 
CPU::update_timers()
{
        if (sound_timer > 0)
                --sound_timer;
        if (delay_timer > 0)
                --delay_timer;
}

void
CPU::execute_op(uint16_t opcode)
{
        // Decode variables from opcode
        uint16_t nnn = opcode & 0xFFF;
        uint8_t nn = opcode & 0xFF;
        uint8_t n = opcode & 0xF; // 4bits

        uint8_t x = (opcode & 0xF00) >> 8;
        uint8_t y = (opcode & 0xF0) >> 4;

        switch (opcode & 0xF000) {
        // 0x0NNN is ignored
        case 0x0000:
                if (opcode == 0x00E0) // Clear display
                        std::ranges::fill_n(framebuf.begin(), 64 * 32, 0);
                else if (opcode == 0x00EE) {
                        pc = stack.pop();
                }
                break;

        case 0x2000:
                stack.push(pc);
                [[fallthrough]];
        case 0x1000:
                pc = nnn;
                break;

        case 0x3000:
                if (reg[x] == nn)
                        pc += 2;
                break;

        case 0x4000:
                if (reg[x] != nn)
                        pc += 2;
                break;

        case 0x5000:
                if (reg[x] == reg[y])
                        pc += 2;
                break;

        case 0x6000:
                reg[x] = nn;
                break;

        case 0x7000:
                reg[x] += nn;
                break;

        case 0x8000:
                // n = 0x000f, nibble used to identify the oper
                al_oper(x, y, n);
                break;

        case 0x9000:
                if (reg[x] != reg[y])
                        pc += 2;
                break;

        case 0xA000:
                index_reg = nnn;
                break;

        case 0xB000:
                pc = nnn + reg[0];
                break;

        case 0xC000:
                reg[x] = distr(rng) & nn;
                break;

        case 0xD000: // Sprites are clipped
                draw_framebuf(x, y, n);
                break;

        case 0xE000:
                if ((opcode & 0xFF) == 0x9E && bus.key_pressed(reg[x]))
                        pc += 2;
                else if ((opcode & 0xFF) == 0xA1 && !bus.key_pressed(reg[x])) 
                        pc += 2;
                break;

        case 0xF000:
                misc_oper(x, nn);
                break;

        default:
                break;
        }
        //print_opcode(opcode);
}

void
CPU::misc_oper(uint8_t x, uint8_t misc_op)
{
        // For 0x55, 0x65
        uint16_t addr { index_reg };

        switch(misc_op) {
        case 0x07:
                reg[x] = delay_timer;
                break;
        case 0x0A:
                // reg[x] = get_key(); blocking?
                break;
        case 0x15:
                delay_timer = reg[x];
                break;
        case 0x18:
                sound_timer = reg[x];
                break;
        case 0x1E:
                index_reg += reg[x];
                break;
        case 0x29:
                index_reg = 0x50 + ((reg[x] & 0xF) * 5);
                break;
        case 0x33: // VX to BCD
        {
                uint8_t value { reg[x] };
                bus.write(index_reg, value / 100);
                bus.write(index_reg + 1, (value / 10) % 10);
                bus.write(index_reg + 2, value % 10);
                break;
        }
        case 0x55:
                for (auto i{0U}; i <= x; ++i, ++addr)
                        bus.write(addr, reg[i]);
                break;
        case 0x65:
                for (auto i{0U}; i <= x; ++i, ++addr)
                        reg[i] = bus.read(addr);
                break;
        default:
                break;
        }
}

void
CPU::al_oper(uint8_t x, uint8_t y, uint8_t al_op)
{
        uint8_t flag {};

        switch(al_op) {
        case 0x0:
                reg[x] = reg[y];
                break;
        case 0x1:
                reg[x] |= reg[y];
                break;
        case 0x2:
                reg[x] &= reg[y];
                break;
        case 0x3:
                reg[x] ^= reg[y];
                break;
        case 0x4:
                // Overflow check
                flag = (reg[x] + reg[y] > UINT8_MAX) ? 1 : 0;
                reg[x] += reg[y];
                reg[0xF] = flag;
                break;
        case 0x5:
                // Underflow check
                flag = (reg[x] >= reg[y]) ? 1 : 0;
                reg[x] -= reg[y];
                reg[0xF] = flag;
                break;
        case 0x6:
                // Legacy cmd
                // reg[x] = reg[y];
                flag = reg[x] & 0x1;
                reg[x] >>= 1;
                reg[0xF] = flag;
                break;
        case 0x7:
                // Underflow check
                flag = (reg[y] >= reg[x]) ? 1 : 0;
                reg[x] = reg[y] - reg[x];
                reg[0xF] = flag;
                break;
        case 0xE:
                // Legacy cmd
                // reg[x] = reg[y];
                flag = (reg[x] >> 7) & 0x1;
                reg[x] <<= 1;
                reg[0xF] = flag;
                break;
        default:
                break;
        }
}
