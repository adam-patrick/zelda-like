#pragma once
#include <SDL2/SDL.h>

namespace zelda::game
{
    class Camera
    {
    public:
        int width  = 0;
        int height = 0;
        float x = 0.f;
        float y = 0.f;

        Camera() = default;

        void follow(float targetX, float targetY, int mapWpx, int mapHpx);
        SDL_Rect getViewRect() const;
    };
}
