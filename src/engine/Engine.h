#pragma once

#include <SDL.h>
#include <cstdint>
#include "../game/Player.h"

namespace zelda::engine
{
    struct Vector2
    {
        float x;
        float y;
    };

    class Engine
    {
    public:
        Engine();
        ~Engine();

        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        bool init(const char* title, int windowWidth, int windowHeight, bool fullscreen);
        void run();
        void shutdown();

    private:
        void processInput();
        void updateFixedStep();
        void renderFrame();
        void capFrameRate(uint32_t frameStartMs);

        bool m_running{false};

        SDL_Window*   m_window{nullptr};
        SDL_Renderer* m_renderer{nullptr};

        // Timing config
        static constexpr int   TARGET_FPS      = 60;
        static constexpr float TARGET_DT_SEC   = 1.0f / static_cast<float>(TARGET_FPS);
        static constexpr uint32_t FRAME_MS_CAP = 1000 / TARGET_FPS;

        float    m_accumulatorSec{0.0f};
        uint32_t m_lastTickMs{0};

        // --- Game state ---
        zelda::game::Player m_player;

        // Window size (for later camera work / bounds)
        int m_windowWidth{0};
        int m_windowHeight{0};

        // Input state for this frame (pressed or not)
        bool m_inputUp{false};
        bool m_inputDown{false};
        bool m_inputLeft{false};
        bool m_inputRight{false};
    };
}
