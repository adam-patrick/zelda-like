#pragma once

#include <SDL.h>
#include <cstdint>
#include <vector>

#include "../game/Player.h"
#include "../game/Enemy.h"
#include "../game/TileMap.h"
#include "../game/PlayerAttack.h"

namespace zelda::engine
{
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
        void movePlayerWithCollision(float dtSec);

        // combat helpers
        void spawnPlayerAttack();
        void updateAttacks(float dtSec);
        void handleCombat();

        bool m_running{false};
        SDL_Window*   m_window{nullptr};
        SDL_Renderer* m_renderer{nullptr};

        static constexpr int   TARGET_FPS      = 60;
        static constexpr float TARGET_DT_SEC   = 1.0f / static_cast<float>(TARGET_FPS);
        static constexpr uint32_t FRAME_MS_CAP = 1000 / TARGET_FPS;

        float    m_accumulatorSec{0.0f};
        uint32_t m_lastTickMs{0};

        zelda::game::Player  m_player;
        zelda::game::Enemy   m_enemy;
        zelda::game::TileMap m_map;
        std::vector<zelda::game::PlayerAttack> m_attacks;

        int m_windowWidth{0};
        int m_windowHeight{0};

        bool m_inputUp{false};
        bool m_inputDown{false};
        bool m_inputLeft{false};
        bool m_inputRight{false};
    };
}
