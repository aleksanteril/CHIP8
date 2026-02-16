#ifndef CPU_HPP
#define CPU_HPP

#include "bus.hpp"

#include <array>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <random>

class Stack // LIFO, ptr points to empty slot
{
      public:
        void push(uint16_t value) {
                if (stack_ptr >= 16)
                        throw std::overflow_error("Stack overflow");
                stack[stack_ptr++] = value;
        }
        uint16_t pop() {
                if (stack_ptr == 0)
                        throw std::overflow_error("Stack underflow");
                return stack[--stack_ptr];
        }
      private:
        std::array<uint16_t, 16> stack;
        uint8_t stack_ptr { 0 };
};

class CPU
{
      public:
        explicit CPU(Bus bus);
        void cycle();
        void update_timers(); // Poll this at ~ 60hz
        std::array<bool, 64 * 32>& framebuf_ref();

      private:
        Bus bus;
        Stack stack;

        void execute_op(uint16_t opcode);
        void al_oper(uint8_t x, uint8_t y, uint8_t arithmetic_op);
        void misc_oper(uint8_t x, uint8_t nn);

        std::array<uint8_t, 16> reg; // Registers V0-VF
        uint16_t index_reg {};
        uint16_t pc { 0x200 };

        uint8_t delay_timer {};
        uint8_t sound_timer {};

        std::default_random_engine rng { std::random_device{}() };
        std::uniform_int_distribution<int> distr{ 0, 255 };

        std::array<bool, 64 * 32> framebuf; // Framebuffer to draw to
        void draw_framebuf(uint8_t x, uint8_t y, uint8_t n);
};

#endif
