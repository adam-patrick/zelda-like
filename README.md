# Zelda-Like (C++ / SDL2)

A **2D top-down action adventure game** inspired by *The Legend of Zelda: A Link to the Past*, written entirely in **modern C++** using **SDL2**.

This project is being built step-by-step **with the help of ChatGPT**, focusing on clean architecture, readability, and an educational approach to writing a simple yet functional 2D game engine from scratch.

---

## 🎯 Current Progress

### ✅ Completed Milestones
1. **Milestone 1 – Core Engine Loop**
   - SDL2 initialization (window, renderer, input)
   - Fixed timestep 60 FPS logic
   - Clean shutdown

2. **Milestone 2 – Player Movement**
   - Player entity rendered as a sprite (yellow square)
   - Keyboard input for 4-directional movement
   - Deterministic update loop

3. **Milestone 3 – Tilemap & Collision**
   - Simple 15x10 tile map (16x16 tiles)
   - Blue walls and dark floor
   - Player collides with solid tiles

4. **Milestone 4 – Enemy Entity**
   - Enemy entity with HP, rendered as red square
   - Static placement in world

5. **Milestone 5 – Player Attack / Combat**
   - Spacebar/J triggers a sword swing (yellow hitbox)
   - Enemies take damage and despawn at 0 HP
   - Attack cooldown and duration system

---

## 🧭 Upcoming Milestones

6. **Camera & Room Transitions** – Smooth scrolling between rooms  
7. **HUD & Hearts System** – Player HP, attack UI, and status icons  
8. **Dialogue Boxes & Interaction** – NPC conversations and signs  
9. **Save / Load System** – Basic persistence for player progress  
10. **Audio** – Sound effects and music via SDL_mixer

---

## 🛠️ Build Instructions

### 🐧 Linux / WSL
```bash
sudo apt install build-essential cmake libsdl2-dev
mkdir build && cd build
cmake ..
cmake --build .
./zelda_like
```

### 🪟 Windows (future support)
SDL2 runtime and development libraries will be bundled under `external/` for MSVC or MinGW builds.

---

## 📁 Project Structure

```
src/
├─ engine/        → window, renderer, input, timing
├─ game/          → player, enemies, maps, combat
└─ main.cpp       → entry point
```

---

## 📘 About the Collaboration

This project is being developed with assistance from **ChatGPT (OpenAI)** to serve as a transparent, learning-friendly example of building a retro-style game engine from scratch — emphasizing clarity, maintainability, and incremental design.

---

© 2025 Adam Patrick.  
Built with ❤️ + C++ + SDL2 + ChatGPT.
