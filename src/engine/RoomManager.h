#pragma once
#include <vector>
#include <array>
#include <algorithm>
#include "TileMap.h"

namespace zelda::game
{
    // Milestone 7:
    // We now support a tiny 2x2 dungeon grid:
    // (0,0) (1,0)
    // (0,1) (1,1)
    //
    // room coords: (m_roomX, m_roomY)
    //
    // We generate all 4 rooms procedurally for now.
    // Each room gets a different "tintId" so we can color it differently when rendering.

    class RoomManager
    {
    public:
        RoomManager()
        : m_roomX(0)
        , m_roomY(0)
        {
            for (auto &slot : m_rooms)
                slot.valid = false;
        }

        struct RoomSlot {
            bool valid = false;
            TileMap map;
            int tintId = 0; // 0,1,2,3 for visual variation
        };

        // build a 2x2 block of rooms with borders + door gaps
        void debugInitRooms(int w, int h)
        {
            // Precompute base tiles for a single room
            std::vector<int> base;
            base.resize(w * h);

            for (int i = 0; i < w * h; ++i)
                base[i] = 0; // floor

            // Add solid border walls (tile = 1)
            for (int x = 0; x < w; ++x)
            {
                base[x] = 1;                   // top
                base[x + (h - 1) * w] = 1;     // bottom
            }
            for (int y = 0; y < h; ++y)
            {
                base[0 + y * w] = 1;           // left
                base[(w - 1) + y * w] = 1;     // right
            }

            // Carve gaps (doors) in all 4 directions:
            // vertical door gap centered horizontally
            const int doorXStart = 6;
            const int doorWidth  = 3;
            for (int dx = 0; dx < doorWidth; ++dx)
            {
                // north/south doors
                base[(doorXStart + dx) + 0 * w] = 0;           // top gap
                base[(doorXStart + dx) + (h - 1) * w] = 0;     // bottom gap
            }

            // horizontal door gap centered vertically
            const int doorYStart = 3;
            const int doorHeight = 2;
            for (int dy = 0; dy < doorHeight; ++dy)
            {
                // west/east doors
                base[0 + (doorYStart + dy) * w] = 0;           // left gap
                base[(w - 1) + (doorYStart + dy) * w] = 0;     // right gap
            }

            // Now clone that room into a 2x2 grid with different tintIds
            // Layout index = (y * 2 + x)
            for (int y = 0; y < 2; ++y)
            {
                for (int x = 0; x < 2; ++x)
                {
                    int idx = y * 2 + x;
                    m_rooms[idx].valid = true;
                    m_rooms[idx].map.load(w, h, base);
                    m_rooms[idx].tintId = idx; // 0,1,2,3
                }
            }

            // start in top-left room (0,0)
            m_roomX = 0;
            m_roomY = 0;
        }

        // Return the active room's tilemap
        TileMap& currentMap()
        {
            return currentSlot().map;
        }

        // For rendering tint, HUD, debugging, etc.
        int currentTintId() const
        {
            return currentSlot().tintId;
        }

        // Where we are in the grid
        int roomX() const { return m_roomX; }
        int roomY() const { return m_roomY; }

        // Movement between rooms:
        void goNorth() { if (m_roomY > 0) m_roomY -= 1; }
        void goSouth() { if (m_roomY < 1) m_roomY += 1; }
        void goWest()  { if (m_roomX > 0) m_roomX -= 1; }
        void goEast()  { if (m_roomX < 1) m_roomX += 1; }

    private:
        // Map a (roomX, roomY) to a slot index
        int idx() const
        {
            // 2 columns, 2 rows
            return m_roomY * 2 + m_roomX;
        }

        RoomSlot& currentSlot()
        {
            return m_rooms[idx()];
        }
        const RoomSlot& currentSlot() const
        {
            return m_rooms[idx()];
        }

        // 2x2 grid => 4 total
        std::array<RoomSlot, 4> m_rooms;

        int m_roomX;
        int m_roomY;
    };
}
