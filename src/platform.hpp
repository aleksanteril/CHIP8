#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include "debug.hpp"
#include <array>

class Platform
{
      public:
        virtual ~Platform() = default;
        virtual void draw_screen(const std::array<bool, 64 * 32>& buffer) = 0;
        virtual void draw_screen(const std::array<bool, 64 * 32>& buffer, struct CPU_State state) = 0;
        virtual void process_events(std::array<bool, 16>& keypad) = 0;
        virtual void play_sound(bool active) = 0;
        bool run { true };

      private:
};


#include <SDL3/SDL.h>

#include <cstdint>
#include <memory>

class SDL3 : public Platform
{
      public:
        SDL3();
        ~SDL3() override;
        void draw_screen(const std::array<bool, 64 * 32>& buffer) override;
        void draw_screen(const std::array<bool, 64 * 32>& buffer, struct CPU_State state) override;
        void process_events(std::array<bool, 16>& keypad) override;
        void play_sound(bool active) override;


      private:
        void draw_state_text(struct CPU_State& state);
        std::array<uint32_t, 64 * 32> pixels;
        static constexpr uint32_t width = 1280;
        static constexpr uint32_t height = 640;

        std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> sdlWindow {nullptr, SDL_DestroyWindow};
        std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> sdlRenderer {nullptr, SDL_DestroyRenderer};
        std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> sdlTexture {nullptr, SDL_DestroyTexture};
        std::unique_ptr<SDL_AudioStream, decltype(&SDL_DestroyAudioStream)> audioStream {nullptr, SDL_DestroyAudioStream};
        SDL_AudioDeviceID audioDevice = 0;
        SDL_Event event;
};

#endif
