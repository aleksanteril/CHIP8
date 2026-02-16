#ifndef RAM_HPP
#define RAM_HPP

#include <array>
#include <cstdint>

class RAM
{
      public:
        uint8_t read(uint16_t addr) const { return ram[addr]; }
        void write(uint16_t addr, uint8_t value)
        {
                if (addr < 4096)
                        ram[addr] = value;
        }

      private:
        static constexpr int size = 4096;
        static constexpr int reserved = 0x200;
        std::array<uint8_t, size> ram;
};

#endif
