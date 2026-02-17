#include "platform.hpp"
#include <algorithm>

SDL3::SDL3()
{
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) 
                throw std::runtime_error("Could not init video");

        // Temporary pointers
        SDL_Window* window{ nullptr };
        SDL_Renderer* render{ nullptr };

        if (!SDL_CreateWindowAndRenderer(
              "Chip8 - AL", width, height, 0, &window, &render))
                throw std::runtime_error("Could not create window");

        sdlWindow.reset(window);
        sdlRenderer.reset(render);

        sdlTexture.reset(SDL_CreateTexture(sdlRenderer.get(),
                                           SDL_PIXELFORMAT_RGBA8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           64,
                                           32));
        if (sdlTexture == nullptr)
                throw std::runtime_error("Error creating texture");

        SDL_SetTextureScaleMode(sdlTexture.get(), SDL_SCALEMODE_NEAREST);

        audioDevice =
          SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
        if (!audioDevice)
                throw std::runtime_error("Could not init audio");

        SDL_AudioSpec spec = { SDL_AUDIO_S16, 1, 44000 };
        audioStream.reset(SDL_CreateAudioStream(&spec, nullptr));
        if (!audioStream)
                throw std::runtime_error("Could not create audio stream");

        SDL_BindAudioStream(audioDevice, audioStream.get());
        SDL_ResumeAudioDevice(audioDevice);
}

SDL3::~SDL3()
{
        SDL_CloseAudioDevice(audioDevice);
        SDL_Quit();
}

void
SDL3::draw_screen(std::array<bool, 64 * 32>& buffer)
{
        // Convert the chip8 framebuf
        std::ranges::transform(buffer, pixels.begin(), [](bool px) {
                return px ? 0xFFFFFFFF : 0x000000FF;
        });

        SDL_UpdateTexture(
          sdlTexture.get(), nullptr, pixels.data(), 64 * sizeof(uint32_t));
        SDL_RenderClear(sdlRenderer.get());
        SDL_RenderTexture(
          sdlRenderer.get(), sdlTexture.get(), nullptr, nullptr);
        SDL_RenderPresent(sdlRenderer.get());
}

// Generate a beep of 500hz with amplitude of 4000
// This works only with the 44000 sample rate
static constexpr auto sqwave = []() {
        std::array<int16_t, 528> samples;
        constexpr int A{ 4000 };

        int i{};
        for (auto& sample : samples) {
                sample = i++ < 44 ? A : A * -1;
                if (i == 88)
                        i = 0;
        }
        return samples;
}();

void
SDL3::play_sound(bool active)
{
        if (!active) { // Empty the stream
                SDL_ClearAudioStream(audioStream.get());
                return;
        }
        
        if (SDL_GetAudioStreamQueued(audioStream.get()) < 2000) {
                SDL_PutAudioStreamData(audioStream.get(),
                                       sqwave.data(),
                                       sqwave.size() * sizeof(int16_t));
        }
}

void
SDL3::process_events(std::array<bool, 16>& keypad)
{
        while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
                quit = true;
                break;
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
                switch (event.key.scancode) {
                case SDL_SCANCODE_1:
                        keypad[0x1] = event.key.down;
                        break;
                case SDL_SCANCODE_2:
                        keypad[0x2] = event.key.down;
                        break;
                case SDL_SCANCODE_3:
                        keypad[0x3] = event.key.down;
                        break;
                case SDL_SCANCODE_4:
                        keypad[0xC] = event.key.down;
                        break;

                case SDL_SCANCODE_Q:
                        keypad[0x4] = event.key.down;
                        break;
                case SDL_SCANCODE_W:
                        keypad[0x5] = event.key.down;
                        break;
                case SDL_SCANCODE_E:
                        keypad[0x6] = event.key.down;
                        break;
                case SDL_SCANCODE_R:
                        keypad[0xD] = event.key.down;
                        break;

                case SDL_SCANCODE_A:
                        keypad[0x7] = event.key.down;
                        break;
                case SDL_SCANCODE_S:
                        keypad[0x8] = event.key.down;
                        break;
                case SDL_SCANCODE_D:
                        keypad[0x9] = event.key.down;
                        break;
                case SDL_SCANCODE_F:
                        keypad[0xE] = event.key.down;
                        break;

                case SDL_SCANCODE_Z:
                        keypad[0xA] = event.key.down;
                        break;
                case SDL_SCANCODE_X:
                        keypad[0x0] = event.key.down;
                        break;
                case SDL_SCANCODE_C:
                        keypad[0xB] = event.key.down;
                        break;
                case SDL_SCANCODE_V:
                        keypad[0xF] = event.key.down;
                        break;
                default:
                        break;
                }
                break;
        default:
                break;
        }
        }
}
