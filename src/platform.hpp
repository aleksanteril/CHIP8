#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <array>
#include <cstdint>
#include <stdexcept>

class Platform
{
      public:
        virtual ~Platform() = default;
        virtual void draw_screen(std::array<bool, 64 * 32>& buffer) = 0;
        virtual void process_events() = 0;
        virtual void sleep(uint32_t milliseconds) = 0;
        bool quit { false };

      private:
};

class SDL3 : public Platform
{
      public:
        SDL3();
        ~SDL3() override;
        void draw_screen(std::array<bool, 64 * 32>& buffer) override;
        void process_events() override;
        void sleep(uint32_t milliseconds) override;


      private:
        static constexpr uint32_t width = 640;
        static constexpr uint32_t height = 320;
        SDL_Window* sdlWindow = nullptr;
        SDL_Renderer* sdlRenderer = nullptr;
        SDL_Texture* sdlTexture = nullptr;
        SDL_Event event;
};

#endif
