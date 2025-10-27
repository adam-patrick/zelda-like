#include "Engine.h"
#include <cstdio>
#include <algorithm> // std::min

namespace zelda::engine
{
    Engine::Engine()
    {
    }

    Engine::~Engine()
    {
        shutdown();
    }

    bool Engine::init(const char* title, int windowWidth, int windowHeight, bool fullscreen)
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0)
        {
            std::printf("SDL_Init failed: %s\n", SDL_GetError());
            return false;
        }

        Uint32 flags = SDL_WINDOW_SHOWN;
        if (fullscreen)
        {
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }

        m_window = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            windowWidth,
            windowHeight,
            flags
        );

        if (!m_window)
        {
            std::printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
            shutdown();
            return false;
        }

        m_renderer = SDL_CreateRenderer(
            m_window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
        );

        if (!m_renderer)
        {
            std::printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
            shutdown();
            return false;
        }

        m_windowWidth  = windowWidth;
        m_windowHeight = windowHeight;

        // Put player roughly in the center of the initial window.
        m_player.x = (m_windowWidth  / 2.0f) - (zelda::game::Player::WIDTH  / 2.0f);
        m_player.y = (m_windowHeight / 2.0f) - (zelda::game::Player::HEIGHT / 2.0f);

        // Setup timing
        m_lastTickMs = SDL_GetTicks();
        m_accumulatorSec = 0.0f;

        m_running = true;
        return true;
    }

    void Engine::run()
    {
        while (m_running)
        {
            uint32_t frameStartMs = SDL_GetTicks();

            processInput();

            uint32_t nowMs = SDL_GetTicks();
            uint32_t frameDeltaMs = nowMs - m_lastTickMs;
            m_lastTickMs = nowMs;

            float frameDeltaSec = static_cast<float>(frameDeltaMs) / 1000.0f;
            m_accumulatorSec += frameDeltaSec;

            if (m_accumulatorSec > 0.25f)
            {
                m_accumulatorSec = 0.25f;
            }

            while (m_accumulatorSec >= TARGET_DT_SEC)
            {
                updateFixedStep();
                m_accumulatorSec -= TARGET_DT_SEC;
            }

            renderFrame();

            capFrameRate(frameStartMs);
        }
    }

    void Engine::shutdown()
    {
        if (m_renderer)
        {
            SDL_DestroyRenderer(m_renderer);
            m_renderer = nullptr;
        }

        if (m_window)
        {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }

        SDL_Quit();
    }

    void Engine::processInput()
    {
        // Handle events like quit/esc
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    m_running = false;
                    break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE)
                    {
                        m_running = false;
                    }
                    break;
                default:
                    break;
            }
        }

        // Poll current keyboard state for arrows
        const Uint8* keys = SDL_GetKeyboardState(nullptr);

        m_inputUp    = (keys[SDL_SCANCODE_UP]    != 0);
        m_inputDown  = (keys[SDL_SCANCODE_DOWN]  != 0);
        m_inputLeft  = (keys[SDL_SCANCODE_LEFT]  != 0);
        m_inputRight = (keys[SDL_SCANCODE_RIGHT] != 0);
    }

    void Engine::updateFixedStep()
    {
        // Push current input state into player for this tick
        m_player.moveUp    = m_inputUp;
        m_player.moveDown  = m_inputDown;
        m_player.moveLeft  = m_inputLeft;
        m_player.moveRight = m_inputRight;

        // Advance player by fixed dt
        m_player.update(TARGET_DT_SEC);

        // Later:
        // - world collision
        // - camera updates
        // - enemy logic
    }

    void Engine::renderFrame()
    {
        // Clear background
        SDL_SetRenderDrawColor(m_renderer, 32, 32, 48, 255);
        SDL_RenderClear(m_renderer);

        // Draw player
        m_player.render(m_renderer);

        SDL_RenderPresent(m_renderer);
    }

    void Engine::capFrameRate(uint32_t frameStartMs)
    {
        uint32_t frameTimeMs = SDL_GetTicks() - frameStartMs;
        if (frameTimeMs < FRAME_MS_CAP)
        {
            SDL_Delay(FRAME_MS_CAP - frameTimeMs);
        }
    }

} // namespace zelda::engine
