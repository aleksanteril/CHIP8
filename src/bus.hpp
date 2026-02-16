#ifndef BUS_HPP
#define BUS_HPP

#include "ram.hpp"
#include <cstdint>
#include <fstream>

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
                for (uint16_t addr{ 0x200 }; file.get(byte); ++addr)
                        ram.write(addr, static_cast<uint8_t>(byte));
        }

      private: // Bus components
        RAM ram;
};

#endif
