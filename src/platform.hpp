#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <array>
#include <cstdint>
#include <stdexcept>
#include <memory>

class Platform
{
      public:
        virtual ~Platform() = default;
        virtual void draw_screen(std::array<bool, 64 * 32>& buffer) = 0;
        virtual void process_events(std::array<bool, 16>& keypad) = 0;
        virtual void play_sound(bool active) = 0;
        bool quit { false };

      private:
};

class SDL3 : public Platform
{
      public:
        SDL3();
        ~SDL3() override;
        void draw_screen(std::array<bool, 64 * 32>& buffer) override;
        void process_events(std::array<bool, 16>& keypad) override;
        void play_sound(bool active) override;


      private:
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
