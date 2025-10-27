#include "Enemy.h"

namespace zelda::game
{
    Enemy::Enemy()
    {
        x = 120.0f;
        y = 80.0f;
        maxHp = 3;
        hp = maxHp;
    }

    void Enemy::render(SDL_Renderer* renderer) const
    {
        if (hp <= 0)
            return;

        SDL_Rect rect{ static_cast<int>(x), static_cast<int>(y), WIDTH, HEIGHT };
        SDL_SetRenderDrawColor(renderer, 180, 40, 40, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_Rect Enemy::getBounds() const
    {
        SDL_Rect rect{ static_cast<int>(x), static_cast<int>(y), WIDTH, HEIGHT };
        return rect;
    }
}
