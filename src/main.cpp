
#include "platform.hpp"
#include "bus.hpp"
#include "cpu.hpp"

#include <fstream>
#include <iostream>

int
main(int argc, char* argv[])
{
        Bus bus;
        if (std::ifstream file(argv[1], std::ios::binary); file.is_open()) {
        //if (std::ifstream file("chip8-logo.ch8", std::ios::binary); file.is_open()) {
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
                platform.sleep(1 / 60);
        }

        return 0;
}
