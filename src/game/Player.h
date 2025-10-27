#pragma once

#include <SDL.h>

namespace zelda::game
{
    class Player
    {
    public:
        Player();

        // position in world space (pixels for now)
        float x;
        float y;

        // size in pixels for rendering & collision debug
        static constexpr int WIDTH  = 16;
        static constexpr int HEIGHT = 16;

        // movement speed in pixels per second
        static constexpr float SPEED_PPS = 80.0f; // tweakable

        // Input state for movement (set by Engine each frame before updateFixedStep)
        bool moveUp    = false;
        bool moveDown  = false;
        bool moveLeft  = false;
        bool moveRight = false;

        // Called at fixed 60Hz by Engine
        // dtSec will be ~1/60 = 0.016666...
        void update(float dtSec);

        // Called every render to draw the player
        void render(SDL_Renderer* renderer) const;
    };
}
