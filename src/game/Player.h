#pragma once

#include <SDL.h>

namespace zelda::game
{
    class Player
    {
    public:
        Player();

        float x;
        float y;

        static constexpr int WIDTH  = 16;
        static constexpr int HEIGHT = 16;

        static constexpr float SPEED_PPS = 80.0f;

        bool moveUp    = false;
        bool moveDown  = false;
        bool moveLeft  = false;
        bool moveRight = false;

        // Return desired movement velocity in pixels per second
        SDL_FPoint computeVelocity() const;

        // Draw player
        void render(SDL_Renderer* renderer) const;
    };
}
