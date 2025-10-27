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
        float vx = 0.0f;
        float vy = 0.0f;

        if (moveUp)    vy -= SPEED_PPS;
        if (moveDown)  vy += SPEED_PPS;
        if (moveLeft)  vx -= SPEED_PPS;
        if (moveRight) vx += SPEED_PPS;

        return SDL_FPoint{vx, vy};
    }

    void Player::render(SDL_Renderer* renderer) const
    {
        SDL_Rect rect;
        rect.x = static_cast<int>(x);
        rect.y = static_cast<int>(y);
        rect.w = WIDTH;
        rect.h = HEIGHT;

        SDL_SetRenderDrawColor(renderer, 200, 220, 64, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}
