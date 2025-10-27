#include "Camera.h"
#include "Engine.h" // only if you still reference Player::WIDTH/HEIGHT here
#include <algorithm>

using namespace zelda::game;

void Camera::follow(float targetX, float targetY, int mapWpx, int mapHpx)
{
    // If Engine.h no longer provides Player::WIDTH/HEIGHT, hard-code or pass them in.
    constexpr int playerW = 14;
    constexpr int playerH = 14;

    float halfW = width  * 0.5f;
    float halfH = height * 0.5f;
    float camX  = targetX + playerW / 2.0f  - halfW;
    float camY  = targetY + playerH / 2.0f - halfH;

    camX = std::clamp(camX, 0.0f, std::max(0.0f, (float)mapWpx - width));
    camY = std::clamp(camY, 0.0f, std::max(0.0f, (float)mapHpx - height));

    x = camX;
    y = camY;
}

SDL_Rect Camera::getViewRect() const
{
    SDL_Rect r;
    r.x = (int)x;
    r.y = (int)y;
    r.w = width;
    r.h = height;
    return r;
}
