#include "Player.h"

namespace zelda::game
{
    Player::Player()
    {
        // Start in middle-ish of the screen for now.
        // We'll let the Engine position us explicitly after creating us,
        // but this is a reasonable default.
        x = 100.0f;
        y = 100.0f;
    }

    void Player::update(float dtSec)
    {
        float vx = 0.0f;
        float vy = 0.0f;

        if (moveUp)    { vy -= SPEED_PPS; }
        if (moveDown)  { vy += SPEED_PPS; }
        if (moveLeft)  { vx -= SPEED_PPS; }
        if (moveRight) { vx += SPEED_PPS; }

        // NOTE: At some point we'll forbid diagonal = faster, but not yet.
        x += vx * dtSec;
        y += vy * dtSec;

        // Later we'll clamp to world bounds / collide with walls.
    }

    void Player::render(SDL_Renderer* renderer) const
    {
        // Render as a filled rectangle.
        SDL_Rect rect;
        rect.x = static_cast<int>(x);
        rect.y = static_cast<int>(y);
        rect.w = WIDTH;
        rect.h = HEIGHT;

        // temp color: bright-ish
        SDL_SetRenderDrawColor(renderer, 200, 220, 64, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}
