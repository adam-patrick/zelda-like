#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>

#include "RoomManager.h"
#include "TileMap.h"
#include "Camera.h"
#include "TextureManager.h"

namespace zelda::game {

    struct PlayerAttack {
        SDL_Rect rect;
        float lifetime;
        PlayerAttack(int x,int y,int w,int h)
        {
            rect = {x,y,w,h};
            lifetime = 0.15f;
        }
        bool isExpired() const { return lifetime <= 0.0f; }
    };

    struct Player {
        static constexpr int WIDTH  = 14;
        static constexpr int HEIGHT = 14;

        float x = 0.f;
        float y = 0.f;

        bool moveUp = false;
        bool moveDown = false;
        bool moveLeft = false;
        bool moveRight = false;

        bool attacking = false;
        float attackCooldown = 0.0f;

        float speed = 80.0f; // px/sec

        SDL_FPoint computeVelocity() const
        {
            float vx = 0.f;
            float vy = 0.f;
            if (moveUp)    vy -= 1.f;
            if (moveDown)  vy += 1.f;
            if (moveLeft)  vx -= 1.f;
            if (moveRight) vx += 1.f;

            float mag2 = vx*vx + vy*vy;
            if (mag2 > 1.0f)
            {
                float invMag = 1.0f / SDL_sqrtf(mag2);
                vx *= invMag;
                vy *= invMag;
            }

            vx *= speed;
            vy *= speed;

            SDL_FPoint out{vx, vy};
            return out;
        }
    };

    struct Enemy {
        static constexpr int WIDTH  = 14;
        static constexpr int HEIGHT = 14;

        float x = 0.f;
        float y = 0.f;
        int hp = 3;

        SDL_Rect getBounds() const
        {
            return SDL_Rect{
                static_cast<int>(x),
                static_cast<int>(y),
                WIDTH,
                HEIGHT
            };
        }
    };
}

namespace zelda::engine
{
    class Engine
    {
    public:
        Engine();
        ~Engine();

        bool init(const char *title, int windowWidth, int windowHeight, bool fullscreen);
        void run();
        void shutdown();

    private:
        void processInput();
        void updateFixedStep();
        void movePlayerWithCollision(float dtSec);
        void handleRoomTransition();
        void spawnPlayerAttack();
        void updateAttacks(float dtSec);
        void handleCombat();
        void renderFrame();
        void capFrameRate(uint32_t frameStartMs);

        // SDL
        SDL_Window   *m_window   = nullptr;
        SDL_Renderer *m_renderer = nullptr;

        int m_windowWidth  = 0;
        int m_windowHeight = 0;

        // timing
        uint32_t m_lastTickMs     = 0;
        float    m_accumulatorSec = 0.0f;
        bool     m_running        = false;

        // fixed timestep config
        static constexpr float TARGET_DT_SEC    = 1.0f / 60.0f;
        static constexpr uint32_t FRAME_MS_CAP = 1000 / 60; // ~16ms

        // input state
        bool m_inputUp    = false;
        bool m_inputDown  = false;
        bool m_inputLeft  = false;
        bool m_inputRight = false;

        // game state
        zelda::game::Player m_player;
        zelda::game::Enemy  m_enemy;
        zelda::game::Camera m_camera;
        std::vector<zelda::game::PlayerAttack> m_attacks;

        zelda::game::RoomManager    m_rooms;
        zelda::game::TextureManager m_textures; // texture cache
    };
}
