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
SDL3::draw_state_text(struct CPU_State& state)
{
        constexpr double scale{ 2 };
        SDL_SetRenderScale(sdlRenderer.get(), scale, scale);
        SDL_SetRenderDrawColor(sdlRenderer.get(), 255, 255, 255, 255);

        //Print stack values
        SDL_RenderDebugTextFormat(sdlRenderer.get(), 1000 / scale, 10, "ST_PTR [ 0x%03X ]", state.stack_ptr);
        for(auto i{ 0 }; i < state.stack_ptr; ++i) 
                SDL_RenderDebugTextFormat(sdlRenderer.get(), 1000 / scale, 10*(i+2), "[ 0x%03X ]", state.stack[i]);
        

        // Draw other state values
        SDL_RenderDebugTextFormat(sdlRenderer.get(),
                                  10,
                                  10,
                                  "PC [ 0x%03X ], I [ 0x%03X ], ST [ 0x%03X ], DT [ 0x%03X ]",
                                  state.pc,
                                  state.index_reg,
                                  state.sound_timer,
                                  state.delay_timer);

        // Draw register values
        for (auto i{ 0 }; i < 4; ++i)
                SDL_RenderDebugTextFormat(sdlRenderer.get(),
                                          10,
                                          i*10+20,
                                          "V%X 0x%03X, V%X 0x%03X, V%X 0x%03X, V%X 0x%03X",
                                          i, state.registers[i],
                                          i + 4, state.registers[i + 4],
                                          i + 8, state.registers[i + 8],
                                          i + 12, state.registers[i + 12]);

        SDL_SetRenderScale(sdlRenderer.get(), 1, 1);
}

void
SDL3::draw_screen(const std::array<bool, 64 * 32>& buffer, struct CPU_State state)
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
        draw_state_text(state);
        SDL_RenderPresent(sdlRenderer.get());
}

void
SDL3::draw_screen(const std::array<bool, 64 * 32>& buffer)
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
