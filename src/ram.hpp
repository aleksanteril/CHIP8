#ifndef RAM_HPP
#define RAM_HPP

#include <array>
#include <cstdint>

class RAM
{
      public:
        uint8_t read(uint16_t addr) const { 
                if (addr < size)
                        return ram[addr]; 
                return 0;
        }
        void write(uint16_t addr, uint8_t value)
        {
                if (addr < size)
                        ram[addr] = value;
        }

      private:
        static constexpr int size = 4096;
        std::array<uint8_t, size> ram {};
};

#endif
