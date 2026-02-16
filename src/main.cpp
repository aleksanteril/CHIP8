
#include "platform.hpp"
#include "bus.hpp"
#include "cpu.hpp"

#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>

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

        auto time_s = std::chrono::steady_clock::now();
        constexpr std::chrono::microseconds frametime(16667);

        CPU cpu(bus);
        SDL3 platform;

        while(!platform.quit) {
                time_s += frametime; // Calculate sleep time 60 hz
                std::this_thread::sleep_until(time_s);

                platform.process_events(bus.keypad());
                // 540 Hz, 9 cycles per loop
                for (auto i { 0 }; i < 9; ++i)
                        cpu.cycle();

                cpu.update_timers();
                platform.play_sound(cpu.sound_active());
                platform.draw_screen(cpu.framebuf_ref());
        }

        return 0;
}
