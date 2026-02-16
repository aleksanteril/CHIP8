#include "platform.hpp"
#include <algorithm>

SDL3::SDL3()
{
        if (!SDL_Init(SDL_INIT_VIDEO))
                throw std::runtime_error("Could not init video");

        if (!SDL_CreateWindowAndRenderer(
              "Chip8 - AL", width, height, 0, &sdlWindow, &sdlRenderer))
                throw std::runtime_error("Could not create window");

        sdlTexture = SDL_CreateTexture(sdlRenderer,
                                       SDL_PIXELFORMAT_RGBA8888,
                                       SDL_TEXTUREACCESS_STREAMING,
                                       64,
                                       32);
        if (sdlTexture == nullptr)
                throw std::runtime_error("Error creating texture");

        SDL_SetTextureScaleMode(sdlTexture, SDL_SCALEMODE_NEAREST);
}

SDL3::~SDL3()
{
        SDL_DestroyRenderer(sdlRenderer);
        SDL_DestroyWindow(sdlWindow);
        SDL_Quit();
}

void
SDL3::draw_screen(std::array<bool, 64 * 32>& buffer)
{
        // Convert the chip8 framebuf
        std::array<uint32_t, 64 * 32> pixels;
        std::ranges::transform(buffer, pixels.begin(), [](bool px) {
                return px ? 0xFFFFFFFF : 0x000000FF;
        });

        SDL_UpdateTexture(sdlTexture, nullptr, pixels.data(), 64*sizeof(uint32_t));
        SDL_RenderClear(sdlRenderer);
        SDL_RenderTexture(sdlRenderer, sdlTexture, nullptr, nullptr);
        SDL_RenderPresent(sdlRenderer);
}

void
SDL3::sleep(uint32_t milliseconds)
{
        SDL_Delay(milliseconds);
}

void
SDL3::process_events()
{
        while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT) {
                        quit = true;
                }
        }
}
