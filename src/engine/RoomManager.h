#pragma once
#include <vector>
#include "TileMap.h"

namespace zelda::game
{
    // Very simple 2-room vertical dungeon.
    // Room 0 <-> Room 1.
    class RoomManager
    {
    public:
        RoomManager()
        : m_currentIndex(0)
        {
            m_rooms.reserve(2);
        }

        // Build two simple rooms so the game always has something to render.
        // Both rooms start identical unless you tweak them later.
        void debugInitRooms(int w, int h)
        {
            std::vector<int> tiles;
            tiles.resize(w * h);

            // fill with floor (0)
            std::fill(tiles.begin(), tiles.end(), 0);

            // make a solid wall border (1)
            for (int x = 0; x < w; ++x)
            {
                tiles[x] = 1;                 // top row
                tiles[x + (h - 1) * w] = 1;   // bottom row
            }
            for (int y = 0; y < h; ++y)
            {
                tiles[0 + y * w] = 1;         // left col
                tiles[(w - 1) + y * w] = 1;   // right col
            }

            // carve door gap in top wall (room 0 north exit / room 1 south entrance)
            // We'll knock out a 3-tile gap around the middle on top/bottom
            const int doorXStart = 6;
            const int doorWidth  = 3;
            for (int dx = 0; dx < doorWidth; ++dx)
            {
                // top center hole
                tiles[(doorXStart + dx) + 0 * w] = 0;
                // bottom center hole
                tiles[(doorXStart + dx) + (h - 1) * w] = 0;
            }

            // room 0
            if (m_rooms.size() < 1)
                m_rooms.emplace_back();
            m_rooms[0].load(w, h, tiles);

            // room 1 (clone for now)
            if (m_rooms.size() < 2)
                m_rooms.emplace_back();
            m_rooms[1].load(w, h, tiles);

            // start player in room 0
            m_currentIndex = 0;
        }

        void setCurrentRoomIndex(int idx)
        {
            if (idx < 0) idx = 0;
            if (idx >= (int)m_rooms.size()) idx = (int)m_rooms.size() - 1;
            if (idx < 0) idx = 0;
            m_currentIndex = idx;
        }

        int currentRoomIndex() const
        {
            return m_currentIndex;
        }

        TileMap& currentMap()
        {
            return m_rooms[m_currentIndex];
        }

        // For Milestone 6:
        // going "north" -> next room index (0 -> 1)
        void goNorth()
        {
            if (m_currentIndex + 1 < (int)m_rooms.size())
                m_currentIndex += 1;
        }

        // going "south" -> previous room index (1 -> 0)
        void goSouth()
        {
            if (m_currentIndex - 1 >= 0)
                m_currentIndex -= 1;
        }

    private:
        std::vector<TileMap> m_rooms;
        int m_currentIndex;
    };
}
