#pragma once
#include <vector>
#include <SDL2/SDL.h>

namespace zelda::game
{
    class TileMap
    {
    public:
        static constexpr int TILE_SIZE = 16;

        TileMap() : m_w(0), m_h(0) {}
        TileMap(int w, int h, const std::vector<int>& tiles)
        {
            load(w, h, tiles);
        }

        void load(int w, int h, const std::vector<int>& tiles)
        {
            m_w = w;
            m_h = h;
            m_tiles = tiles;
            if ((int)m_tiles.size() != w * h)
            {
                // safety fallback if caller passed wrong size
                m_tiles.resize(w * h, 0);
            }
        }

        int width() const  { return m_w; }
        int height() const { return m_h; }

        // Return the numeric tile type at (tx, ty).
        // 0 = floor/walkable, 1 = wall/blocked.
        // Out of bounds counts as wall.
        int getTileId(int tx, int ty) const
        {
            if (tx < 0 || ty < 0 || tx >= m_w || ty >= m_h)
                return 1;
            return m_tiles[ty * m_w + tx];
        }

        // Axis-aligned rectangle vs. solid tiles.
        bool rectCollidesSolid(const SDL_Rect& r) const
        {
            // which tile columns/rows does this rect touch?
            int leftTile   = r.x / TILE_SIZE;
            int rightTile  = (r.x + r.w - 1) / TILE_SIZE;
            int topTile    = r.y / TILE_SIZE;
            int bottomTile = (r.y + r.h - 1) / TILE_SIZE;

            for (int ty = topTile; ty <= bottomTile; ++ty)
            {
                for (int tx = leftTile; tx <= rightTile; ++tx)
                {
                    if (getTileId(tx, ty) == 1) // solid wall
                        return true;
                }
            }
            return false;
        }

    private:
        int m_w;
        int m_h;
        std::vector<int> m_tiles;
    };
}
