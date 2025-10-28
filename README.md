# Zelda-Like Engine

A custom 2D adventure engine built in C++17 with SDL2.  
The goal: a hand-coded top-down Zelda-style game, developed milestone by milestone.

================================
Milestone 7 Status (Current Build)
================================

Milestone 7 introduces a full 2×2 dungeon grid and movement between rooms in all four directions.

The engine now:
- Opens an SDL2 window and runs a fixed 60 FPS loop
- Handles keyboard input:
  - Move: W / A / S / D or Arrow Keys
  - Attack: Space or J
  - Quit: Esc
- Moves a player and checks collision against walls
- Spawns attack hitboxes for melee swings
- Has a test enemy that can be hit and removed
- Supports transitions north/south/east/west between rooms
- Has a 2×2 dungeon grid (4 total rooms)
- Applies a unique color tint per room
- Keeps the camera centered on the player and clamped to room bounds
- Renders everything with solid-color rectangles (no textures yet)

This is our stable “multi-room navigation” baseline.

-------------
Project Layout
-------------

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

Summary:

Engine
- Main game loop (init/run/shutdown)
- Handles input, updates, and rendering
- Manages player, enemy, attacks, camera, and room transitions

Camera
- Tracks viewport position
- Follows player while clamping to room bounds

RoomManager
- Now holds a 2×2 grid of rooms instead of a single list
- Provides currentMap(), currentTintId(), and goNorth/ South/ East/ West
- Generates four test rooms with door gaps on each side

TileMap
- Stores tile grid (0 = floor, 1 = wall)
- Provides collision and tile queries

Player / Enemy / Attack
- Player: movement, speed, attack cooldown
- Enemy: solid block with HP (disappears on death)
- Attack: short-lived hitbox rectangles

----------------
Controls
----------------
Move:    W / A / S / D or Arrow Keys  
Attack:  Space or J  
Quit:    Esc  

----------------
Build Instructions (Linux)
----------------
1. Install SDL2 development headers  
   ```bash
   sudo apt install libsdl2-dev
   ```
2. Configure and build  
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```
3. Run  
   ```bash
   ./zelda_like
   ```

CMake expects:
- src/main.cpp
- src/engine/Engine.cpp
- src/engine/Camera.cpp
- src/engine/RoomManager.cpp
and all headers under src/engine/.

----------------
Next Milestone (8)
----------------
- Replace color rectangles with textured sprites and tiles
- Add basic enemy movement and chase behavior
- Begin HUD overlay (player health, enemy HP)
- Introduce a lightweight asset manager for textures

-----------------
License / Notice
-----------------
© 2025 Adam. All Rights Reserved.  
For personal/educational use while developing this project.
