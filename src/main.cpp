
#include "platform.hpp"
#include "bus.hpp"
#include "cpu.hpp"

#include <fstream>
#include <iostream>

#define SLEEP_INTERVAL 17

int
main(int argc, char* argv[])
{
        Bus bus;
        if (std::ifstream file(argv[1], std::ios::binary); file.is_open()) {
                bus.load_rom(file);
        } else {
                std::cout << "File could not be opened\n";
                return 1;
        }

        CPU cpu(std::move(bus));
        SDL3 platform;

        while(!platform.quit) {
                platform.process_events();

                for (auto i { 0 }; i < 10; ++i)
                        cpu.cycle();
                cpu.update_timers();

                platform.draw_screen(cpu.framebuf_ref());
                platform.sleep(SLEEP_INTERVAL); // Poll at 60Hz
        }

        return 0;
}
