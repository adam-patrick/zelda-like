#include "Player.h"

namespace zelda::game
{
    Player::Player()
    {
        x = 64.0f;
        y = 64.0f;
    }

    SDL_FPoint Player::computeVelocity() const
    {
        float vx = 0.0f, vy = 0.0f;
        if (moveUp)    vy -= SPEED_PPS;
        if (moveDown)  vy += SPEED_PPS;
        if (moveLeft)  vx -= SPEED_PPS;
        if (moveRight) vx += SPEED_PPS;
        return {vx, vy};
    }

    void Player::render(SDL_Renderer* renderer) const
    {
        SDL_Rect rect{ static_cast<int>(x), static_cast<int>(y), WIDTH, HEIGHT };
        SDL_SetRenderDrawColor(renderer, 200, 220, 64, 255);
        SDL_RenderFillRect(renderer, &rect);

        if (attacking)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
            SDL_RenderDrawRect(renderer, &rect);
        }
    }
}
