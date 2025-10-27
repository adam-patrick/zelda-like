#pragma once
#include <SDL.h>

namespace zelda::game
{
    struct PlayerAttack
    {
        SDL_Rect rect;     // attack hitbox
        float lifetime;    // seconds remaining

        PlayerAttack(int x, int y, int w, int h)
        {
            rect = {x, y, w, h};
            lifetime = 0.15f; // lasts 0.15 seconds
        }

        bool isExpired() const { return lifetime <= 0.0f; }
    };
}
