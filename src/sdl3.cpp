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
SDL3::process_events(std::array<bool, 16>& keypad)
{
        while (SDL_PollEvent(&event)) {
                switch(event.type)
                {
                case SDL_EVENT_QUIT:
                        quit = true;
                        break;
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP:
                        switch(event.key.scancode) {
                        case SDL_SCANCODE_1: keypad[0x1] = event.key.down; break;
                        case SDL_SCANCODE_2: keypad[0x2] = event.key.down; break;
                        case SDL_SCANCODE_3: keypad[0x3] = event.key.down; break;
                        case SDL_SCANCODE_4: keypad[0xC] = event.key.down; break;

                        case SDL_SCANCODE_Q: keypad[0x4] = event.key.down; break;
                        case SDL_SCANCODE_W: keypad[0x5] = event.key.down; break;
                        case SDL_SCANCODE_E: keypad[0x6] = event.key.down; break;
                        case SDL_SCANCODE_R: keypad[0xD] = event.key.down; break;

                        case SDL_SCANCODE_A: keypad[0x7] = event.key.down; break;
                        case SDL_SCANCODE_S: keypad[0x8] = event.key.down; break;
                        case SDL_SCANCODE_D: keypad[0x9] = event.key.down; break;
                        case SDL_SCANCODE_F: keypad[0xE] = event.key.down; break;

                        case SDL_SCANCODE_Z: keypad[0xA] = event.key.down; break;
                        case SDL_SCANCODE_X: keypad[0x0] = event.key.down; break;
                        case SDL_SCANCODE_C: keypad[0xB] = event.key.down; break;
                        case SDL_SCANCODE_V: keypad[0xF] = event.key.down; break;
                        default: break;
                        }
                        break;
                default:
                        break;
                }
        }
}
