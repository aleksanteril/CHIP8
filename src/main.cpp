
#include "platform.hpp"
#include "bus.hpp"
#include "cpu.hpp"

#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>

#define DEBUG

int
main(int argc, char* argv[])
{
        Bus bus; // Could use a shared ptr for this in the future
        if (std::ifstream file(argv[1], std::ios::binary); file.is_open()) {
                bus.load_rom(file);
        } else {
                std::cout << "File could not be opened\n";
                return 1;
        }

        CPU cpu(bus);
        SDL3 platform;

        // Loop runs at ~500hz, tick is 2ms
        constexpr std::chrono::microseconds cycle_t(1667); // 600hz
        constexpr std::chrono::microseconds other_t(16667); // 60hz

        auto time_s = std::chrono::steady_clock::now();
        auto other_s = std::chrono::steady_clock::now();
        
        while(platform.run) {
                platform.process_events(bus.keypad());
                cpu.cycle();

                // These happen at 60 hz
                auto now = std::chrono::steady_clock::now();
                if (now - other_s > other_t) {
                        cpu.update_timers();
                        platform.play_sound(cpu.sound_active());

                        #ifdef DEBUG
                        platform.draw_screen(cpu.framebuf_ref(), std::move(cpu.state()));
                        #else
                        if (cpu.need_draw()) 
                                platform.draw_screen(cpu.framebuf_ref());
                        #endif
                        
                        other_s += other_t;
                }
                
                time_s += cycle_t;
                std::this_thread::sleep_until(time_s);
        }

        return 0;
}
