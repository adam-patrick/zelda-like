# Zelda-Like (C++ / SDL2)

A 2D top-down action adventure game inspired by *The Legend of Zelda: A Link to the Past*, written entirely in modern C++ using SDL2.

This project is being developed step-by-step **with assistance from ChatGPT**, focusing on building a clean and educational codebase from scratch — no prebuilt engines, no frameworks, only SDL2 for graphics, input, and audio.

## Current Progress

### ✅ Milestones Completed
- **Milestone 1:** Core engine loop (window, input, 60 FPS, quit handling)
- **Milestone 2:** Player rectangle rendering and movement
- **Milestone 3:** Tilemap rendering and collision detection

### ⏳ Upcoming
- **Milestone 4:** Basic enemy with HP and rendering
- **Milestone 5:** Player attack (sword hitbox) and combat system
- **Milestone 6:** Camera and room transitions
- **Milestone 7:** HUD (hearts), dialogue boxes, and save/load

## Build Instructions

### Linux / WSL
```bash
sudo apt install build-essential cmake libsdl2-dev
mkdir build && cd build
cmake ..
cmake --build .
./zelda_like
```

### Windows (later)
SDL2 libraries will be added under `external/` for MSVC or MinGW builds.

---

Developed in collaboration with **ChatGPT (OpenAI)**, following a milestone-driven design and architecture-first approach.
