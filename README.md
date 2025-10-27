# Zelda-Like Engine

A small 2D adventure engine built in C++17 with SDL2. The long-term goal is a classic top-down Zelda-style game, all written by hand (no Unity/Godot). This README describes the current working state at the end of Milestone 6.

================================
Milestone 6 Status (Current Build)
================================

Milestone 6 is our first stable playable slice. The engine now:
- Opens an SDL2 window and runs a main loop
- Uses a fixed timestep update (60 FPS target)
- Reads keyboard input
  - Move: W / A / S / D or Arrow Keys
  - Attack: Space or J
  - Quit: Esc
- Moves a player character and blocks movement against solid tiles
- Spawns and updates a short-lived attack hitbox for basic melee
- Has a test enemy you can hit and "kill"
- Supports transitioning between two rooms (north/south door gaps)
- Has a camera that follows the player and clamps to the room bounds
- Renders using solid-colored rectangles (no textures yet)

This is our engine baseline going forward.

-------------
Project Layout
-------------

All gameplay/engine code currently lives under src/engine/.

src/
  main.cpp
  engine/
    Engine.h
    Engine.cpp
    Camera.h
    Camera.cpp
    RoomManager.h
    RoomManager.cpp
    TileMap.h

Summary of responsibilities:

Engine
- Owns init(), run(), shutdown()
- Handles input, fixed-step update, rendering
- Manages player, enemy, attacks
- Asks RoomManager for the current TileMap
- Calls Camera to follow the player and clamp view
- Handles room transitions (north/south doorway logic)

Camera
- Holds camera x/y and viewport size
- Follows the player position
- Clamps so we don't scroll past the room edges
- Exposes getViewRect() so render code knows what part of the world to draw

RoomManager
- Holds a small list of rooms (right now: 2 test rooms)
- Exposes currentMap()
- goNorth() / goSouth() to change active room index
- debugInitRooms() builds simple rooms in code at startup so there's always something to render

TileMap
- A tile grid for a single room
- Each tile is just an int: 0 = floor (walkable), 1 = wall (blocked)
- rectCollidesSolid() is used for movement blocking
- getTileId() is used for render color
- TILE_SIZE = 16px

Player / Enemy / Attack
- Player is a solid-color rectangle with movement speed, cooldown for attacks, etc.
- Enemy is a solid-color rectangle with HP. When HP <= 0, we push it offscreen.
- PlayerAttack is just a short-lived hitbox rectangle that damages the enemy if they overlap.

Controls
--------
Move:    W / A / S / D or Arrow Keys
Attack:  Space or J
Quit:    Esc

Build Instructions (Linux / SDL2 dev packages)
----------------------------------------------

1. Install SDL2 development headers (example for Debian/Ubuntu style distros):
   sudo apt install libsdl2-dev

2. Configure and build:
   mkdir build
   cd build
   cmake ..
   make

3. Run:
   ./zelda_like

CMake expects these sources:
- src/main.cpp
- src/engine/Engine.cpp
- src/engine/Camera.cpp
- src/engine/RoomManager.cpp
Plus the headers in src/engine/.

---------------------
Next Milestone Goals
---------------------

Planned next steps (Milestone 7+):
- Replace colored rectangles with textured tiles and sprites
- Add east/west room transitions
- Start basic enemy movement / patrol AI
- Optional: HUD (player health, etc.)

-----------------
License / Notice
-----------------

© 2025 Adam. All Rights Reserved.
For personal/educational use while developing this project. Not licensed for redistribution/commercial use yet.
