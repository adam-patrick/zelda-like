#include "TileMap.h"

namespace zelda::game
{
    TileMap::TileMap()
    {
        static constexpr int W = 15;
        static constexpr int H = 10;
        m_width = W;
        m_height = H;
        m_tiles.reserve(W * H);

        for (int y = 0; y < H; ++y)
        {
            for (int x = 0; x < W; ++x)
            {
                bool border = (y == 0 || y == H - 1 || x == 0 || x == W - 1);
                int tileID = border ? 1 : 0;
                if (y == 0 && x == 7)
                    tileID = 0;
                m_tiles.push_back(tileID);
            }
        }
    }

    void TileMap::render(SDL_Renderer* renderer) const
    {
        for (int ty = 0; ty < m_height; ++ty)
        {
            for (int tx = 0; tx < m_width; ++tx)
            {
                int tileID = m_tiles[ty * m_width + tx];
                SDL_Rect r{ tx * TILE_SIZE, ty * TILE_SIZE, TILE_SIZE, TILE_SIZE };

                if (tileID == 1)
                    SDL_SetRenderDrawColor(renderer, 40, 60, 100, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 20, 20, 28, 255);

                SDL_RenderFillRect(renderer, &r);
            }
        }
    }

    bool TileMap::isSolidAtTile(int tx, int ty) const
    {
        if (tx < 0 || ty < 0 || tx >= m_width || ty >= m_height)
            return true;
        return m_tiles[ty * m_width + tx] == 1;
    }

    bool TileMap::rectCollidesSolid(const SDL_Rect& rect) const
    {
        int leftTile   = rect.x / TILE_SIZE;
        int rightTile  = (rect.x + rect.w - 1) / TILE_SIZE;
        int topTile    = rect.y / TILE_SIZE;
        int bottomTile = (rect.y + rect.h - 1) / TILE_SIZE;

        for (int ty = topTile; ty <= bottomTile; ++ty)
            for (int tx = leftTile; tx <= rightTile; ++tx)
                if (isSolidAtTile(tx, ty))
                    return true;

        return false;
    }
}
