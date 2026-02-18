#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <array>
#include <cstdint>

// Snapshot of CPU state
struct CPU_State
{
        const std::array<uint8_t, 16>& registers;
        const std::array<uint16_t, 16>& stack;
        uint8_t stack_ptr;
        uint16_t index_reg;
        uint16_t pc;
        uint8_t delay_timer;
        uint8_t sound_timer;
};

#endif
