#ifndef BUS_HPP
#define BUS_HPP

#include "ram.hpp"
#include <array>
#include <cstdint>
#include <fstream>

constexpr uint16_t RESERVED = 0x200;

class Bus
{
      public:
        uint8_t read(uint16_t addr) const
        {
                if (addr < 0x1000) // RAM Area 0x0 - 0x1000
                        return ram.read(addr);
                // Others maybe
                return 0;
        }

        void write(uint16_t addr, uint8_t value)
        {
                if (addr < 0x1000) // RAM Area 0x0 - 0x1000
                        ram.write(addr, value);
        }

        void load_rom(std::ifstream& file) //*.ch8
        {
                char byte; // Programs start from 0x200
                for (uint16_t addr{ RESERVED }; file.get(byte); ++addr)
                        ram.write(addr, static_cast<uint8_t>(byte));
        }

        // Return reference to keypad "register/array"
        std::array<bool, 16>& keypad() { return keys; }
        bool key_pressed(uint8_t reg)
        {
                return keys[reg & 0xF]; // Considering lowest nibble
        }

      private: // Bus components
        RAM ram;
        // Keypad array here, could be an object
        std::array<bool, 16> keys{};
};

#endif
