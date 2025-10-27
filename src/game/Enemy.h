#pragma once

#include <SDL.h>

namespace zelda::game
{
    class Enemy
    {
    public:
        Enemy();

        float x;
        float y;

        static constexpr int WIDTH  = 16;
        static constexpr int HEIGHT = 16;

        int maxHp;
        int hp;

        void render(SDL_Renderer* renderer) const;
        SDL_Rect getBounds() const;
    };
}
