#include "TileMap.h"

namespace zelda::game
{
    TileMap::TileMap()
    {
        // For now, we define a tiny test room layout:
        // 1 = solid wall
        // 0 = floor
        //
        // We'll do something like a box with an opening.
        //
        // Example 15 x 10 room:
        static constexpr int W = 15;
        static constexpr int H = 10;

        m_width = W;
        m_height = H;
        m_tiles.reserve(W * H);

        // Build the room:
        // Top and bottom rows = wall (1)
        // Left/right walls = 1
        // Everything else = 0
        // We'll leave a doorway at (7,0) just for fun.

        for (int y = 0; y < H; ++y)
        {
            for (int x = 0; x < W; ++x)
            {
                bool border =
                    (y == 0 || y == H - 1 || x == 0 || x == W - 1);

                int tileID = 0;

                if (border)
                {
                    tileID = 1; // solid wall
                }

                // Put a "door" in the top wall
                if (y == 0 && x == 7)
                {
                    tileID = 0;
                }

                m_tiles.push_back(tileID);
            }
        }
    }

    void TileMap::render(SDL_Renderer *renderer) const
    {
        // Draw each tile as a colored rect for now.
        // Wall (1) = dark blue/gray, Floor (0) = darker floor color.

        for (int ty = 0; ty < m_height; ++ty)
        {
            for (int tx = 0; tx < m_width; ++tx)
            {
                int tileID = m_tiles[ty * m_width + tx];

                SDL_Rect r;
                r.x = tx * TILE_SIZE;
                r.y = ty * TILE_SIZE;
                r.w = TILE_SIZE;
                r.h = TILE_SIZE;

                if (tileID == 1)
                {
                    // wall tile
                    SDL_SetRenderDrawColor(renderer, 40, 60, 100, 255);
                }
                else
                {
                    // floor tile
                    SDL_SetRenderDrawColor(renderer, 20, 20, 28, 255);
                }

                SDL_RenderFillRect(renderer, &r);
            }
        }
    }

    bool TileMap::isSolidAtTile(int tx, int ty) const
    {
        if (tx < 0 || ty < 0 || tx >= m_width || ty >= m_height)
        {
            // out of bounds counts as solid
            return true;
        }

        int tileID = m_tiles[ty * m_width + tx];
        return (tileID == 1);
    }

    bool TileMap::rectCollidesSolid(const SDL_Rect &rect) const
    {
        int leftTile = rect.x / TILE_SIZE;
        int rightTile = (rect.x + rect.w - 1) / TILE_SIZE;
        int topTile = rect.y / TILE_SIZE;
        int bottomTile = (rect.y + rect.h - 1) / TILE_SIZE;

        for (int ty = topTile; ty <= bottomTile; ++ty)
        {
            for (int tx = leftTile; tx <= rightTile; ++tx)
            {
                if (isSolidAtTile(tx, ty))
                {
                    SDL_Log("COLLIDE at tile (%d,%d)", tx, ty);
                    return true;
                }
            }
        }

        return false;
    }
}
