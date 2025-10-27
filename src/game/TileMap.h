#pragma once

#include <SDL.h>
#include <vector>
#include <cstdint>

namespace zelda::game
{
    class TileMap
    {
    public:
        static constexpr int TILE_SIZE = 16;

        TileMap();

        int width() const  { return m_width; }
        int height() const { return m_height; }

        void render(SDL_Renderer* renderer) const;

        bool isSolidAtTile(int tx, int ty) const;
        bool rectCollidesSolid(const SDL_Rect& rect) const;

    private:
        int m_width;
        int m_height;
        std::vector<int> m_tiles; // 0=floor, 1=wall
    };
}
