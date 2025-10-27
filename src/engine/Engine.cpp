#include "Engine.h"

using namespace zelda;

namespace zelda::engine
{
    Engine::Engine() {}
    Engine::~Engine() { shutdown(); }

    bool Engine::init(const char *title, int windowWidth, int windowHeight, bool fullscreen)
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0)
            return false;

        Uint32 flags = SDL_WINDOW_SHOWN;
        if (fullscreen)
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

        m_window = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            windowWidth,
            windowHeight,
            flags);
        if (!m_window)
            return false;

        m_renderer = SDL_CreateRenderer(
            m_window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!m_renderer)
            return false;

        m_windowWidth  = windowWidth;
        m_windowHeight = windowHeight;

        // camera starts same size as window
        m_camera.width  = windowWidth;
        m_camera.height = windowHeight;

        // player spawn
        m_player.x = 64.0f;
        m_player.y = 64.0f;

        // enemy so we can see it
        m_enemy.x = 128.0f;
        m_enemy.y = 96.0f;
        m_enemy.hp = 3;

        // --- ROOM SETUP (Milestone 6 baseline) ---
        // create 2 simple rooms with a border wall and door gaps
        m_rooms.debugInitRooms(/*w=*/10, /*h=*/8);

        // sync camera with current room bounds so initial view is valid
        {
            game::TileMap &map = m_rooms.currentMap();
            int mapWidthPx  = map.width()  * game::TileMap::TILE_SIZE;
            int mapHeightPx = map.height() * game::TileMap::TILE_SIZE;
            m_camera.follow(m_player.x, m_player.y, mapWidthPx, mapHeightPx);
        }

        m_lastTickMs     = SDL_GetTicks();
        m_accumulatorSec = 0.0f;
        m_running        = true;
        return true;
    }

    void Engine::run()
    {
        while (m_running)
        {
            uint32_t frameStartMs = SDL_GetTicks();
            processInput();

            uint32_t nowMs = SDL_GetTicks();
            uint32_t frameDeltaMs = nowMs - m_lastTickMs;
            m_lastTickMs = nowMs;

            float frameDeltaSec = static_cast<float>(frameDeltaMs) / 1000.0f;
            m_accumulatorSec += frameDeltaSec;
            if (m_accumulatorSec > 0.25f)
                m_accumulatorSec = 0.25f;

            while (m_accumulatorSec >= TARGET_DT_SEC)
            {
                updateFixedStep();
                m_accumulatorSec -= TARGET_DT_SEC;
            }

            renderFrame();
            capFrameRate(frameStartMs);
        }
    }

    void Engine::shutdown()
    {
        if (m_renderer)
        {
            SDL_DestroyRenderer(m_renderer);
            m_renderer = nullptr;
        }
        if (m_window)
        {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }
        SDL_Quit();
    }

    void Engine::processInput()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                m_running = false;
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
                m_running = false;
        }

        const Uint8 *keys = SDL_GetKeyboardState(nullptr);
        m_inputUp    = keys[SDL_SCANCODE_UP]    || keys[SDL_SCANCODE_W];
        m_inputDown  = keys[SDL_SCANCODE_DOWN]  || keys[SDL_SCANCODE_S];
        m_inputLeft  = keys[SDL_SCANCODE_LEFT]  || keys[SDL_SCANCODE_A];
        m_inputRight = keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D];

        bool attackPressed = keys[SDL_SCANCODE_SPACE] || keys[SDL_SCANCODE_J];
        if (attackPressed && m_player.attackCooldown <= 0.0f)
        {
            m_player.attacking = true;
            m_player.attackCooldown = 0.3f;
            spawnPlayerAttack();
        }
    }

    void Engine::updateFixedStep()
    {
        // movement intent from input
        m_player.moveUp    = m_inputUp;
        m_player.moveDown  = m_inputDown;
        m_player.moveLeft  = m_inputLeft;
        m_player.moveRight = m_inputRight;

        // move with collision
        movePlayerWithCollision(TARGET_DT_SEC);

        // attempt room transition (north/south doors)
        handleRoomTransition();

        // attack cooldown timer
        if (m_player.attackCooldown > 0.0f)
        {
            m_player.attackCooldown -= TARGET_DT_SEC;
            if (m_player.attackCooldown <= 0.0f)
                m_player.attacking = false;
        }

        // camera follows player within current room bounds
        {
            game::TileMap &map = m_rooms.currentMap();
            int mapWidthPx  = map.width()  * game::TileMap::TILE_SIZE;
            int mapHeightPx = map.height() * game::TileMap::TILE_SIZE;
            m_camera.follow(m_player.x, m_player.y, mapWidthPx, mapHeightPx);
        }

        // attacks & combat
        updateAttacks(TARGET_DT_SEC);
        handleCombat();
    }

    void Engine::movePlayerWithCollision(float dtSec)
    {
        game::TileMap &map = m_rooms.currentMap();

        SDL_FPoint vel = m_player.computeVelocity();
        float dx = vel.x * dtSec;
        float dy = vel.y * dtSec;

        if (dx != 0.0f)
        {
            float newX = m_player.x + dx;
            SDL_Rect rectX{
                static_cast<int>(newX),
                static_cast<int>(m_player.y),
                game::Player::WIDTH,
                game::Player::HEIGHT};
            if (!map.rectCollidesSolid(rectX))
                m_player.x = newX;
        }

        if (dy != 0.0f)
        {
            float newY = m_player.y + dy;
            SDL_Rect rectY{
                static_cast<int>(m_player.x),
                static_cast<int>(newY),
                game::Player::WIDTH,
                game::Player::HEIGHT};
            if (!map.rectCollidesSolid(rectY))
                m_player.y = newY;
        }
    }

    void Engine::handleRoomTransition()
    {
        game::TileMap &map = m_rooms.currentMap();

        const int tileSize = game::TileMap::TILE_SIZE;
        const int mapPixW  = map.width()  * tileSize;
        const int mapPixH  = map.height() * tileSize;

        // where we spawn the player when switching between rooms
        const float doorwayX = 7 * tileSize + 4.0f;

        const float topEntranceY    = tileSize * 2.0f;
        const float bottomEntranceY = mapPixH - tileSize * 2.0f;

        SDL_Rect playerRect{
            static_cast<int>(m_player.x),
            static_cast<int>(m_player.y),
            game::Player::WIDTH,
            game::Player::HEIGHT};

        // door trigger zones (a little lenient)
        SDL_Rect northDoorTrigger{
            6 * tileSize,
            -8,
            3 * tileSize,
            16
        };

        SDL_Rect southDoorTrigger{
            6 * tileSize,
            mapPixH - tileSize,
            3 * tileSize,
            tileSize + 8
        };

        // Try NORTH
        {
            if (SDL_HasIntersection(&playerRect, &northDoorTrigger))
            {
                int before = m_rooms.currentRoomIndex();
                m_rooms.goNorth();
                int after  = m_rooms.currentRoomIndex();

                if (after != before)
                {
                    game::TileMap &newMap = m_rooms.currentMap();
                    int newMapPixH = newMap.height() * tileSize;

                    float newBottomEntranceY = newMapPixH - tileSize * 2.0f;
                    m_player.x = doorwayX;
                    m_player.y = newBottomEntranceY;

                    int newMapWidthPx  = newMap.width()  * tileSize;
                    int newMapHeightPx = newMap.height() * tileSize;
                    m_camera.follow(m_player.x, m_player.y, newMapWidthPx, newMapHeightPx);
                }
                return; // stop here so we don't also trigger south same frame
            }
        }

        // Try SOUTH
        {
            if (SDL_HasIntersection(&playerRect, &southDoorTrigger))
            {
                int before = m_rooms.currentRoomIndex();
                m_rooms.goSouth();
                int after  = m_rooms.currentRoomIndex();

                if (after != before)
                {
                    game::TileMap &newMap = m_rooms.currentMap();

                    m_player.x = doorwayX;
                    m_player.y = topEntranceY;

                    int newMapWidthPx  = newMap.width()  * tileSize;
                    int newMapHeightPx = newMap.height() * tileSize;
                    m_camera.follow(m_player.x, m_player.y, newMapWidthPx, newMapHeightPx);
                }
                return;
            }
        }
    }

    void Engine::spawnPlayerAttack()
    {
        const int range = 18;
        int ax = static_cast<int>(m_player.x);
        int ay = static_cast<int>(m_player.y);
        int w = 12, h = 12;

        if (m_inputUp)
            ay -= range;
        else if (m_inputDown)
            ay += game::Player::HEIGHT;
        else if (m_inputLeft)
            ax -= range;
        else if (m_inputRight)
            ax += game::Player::WIDTH;
        else
            ay += game::Player::HEIGHT;

        m_attacks.emplace_back(ax, ay, w, h);
    }

    void Engine::updateAttacks(float dtSec)
    {
        for (auto &atk : m_attacks)
            atk.lifetime -= dtSec;

        m_attacks.erase(
            std::remove_if(
                m_attacks.begin(),
                m_attacks.end(),
                [](const game::PlayerAttack &a)
                {
                    return a.isExpired();
                }),
            m_attacks.end());
    }

    void Engine::handleCombat()
    {
        if (m_enemy.hp <= 0)
            return;

        for (auto &atk : m_attacks)
        {
            SDL_Rect eRect = m_enemy.getBounds();
            if (SDL_HasIntersection(&atk.rect, &eRect))
            {
                m_enemy.hp--;
                if (m_enemy.hp <= 0)
                {
                    // hide enemy offscreen when "dead"
                    m_enemy.x = -1000.0f;
                    m_enemy.y = -1000.0f;
                }
                break;
            }
        }
    }

    void Engine::renderFrame()
    {
        // clear background
        SDL_SetRenderDrawColor(m_renderer, 8, 8, 12, 255);
        SDL_RenderClear(m_renderer);

        game::TileMap &map = m_rooms.currentMap();

        SDL_Rect view = m_camera.getViewRect();
        const int tileSize = game::TileMap::TILE_SIZE;
        int mapPxW = map.width() * tileSize;
        int mapPxH = map.height() * tileSize;

        // center map if it's smaller than the window
        int offsetX = (mapPxW < m_camera.width)  ? (m_camera.width  - mapPxW) / 2 : 0;
        int offsetY = (mapPxH < m_camera.height) ? (m_camera.height - mapPxH) / 2 : 0;

        bool inRoom1 = (m_rooms.currentRoomIndex() == 1);

        // draw tiles
        for (int ty = 0; ty < map.height(); ++ty)
        {
            for (int tx = 0; tx < map.width(); ++tx)
            {
                int tileID = map.getTileId(tx, ty);

                SDL_Rect r{
                    tx * tileSize - view.x + offsetX,
                    ty * tileSize - view.y + offsetY,
                    tileSize,
                    tileSize};

                if (tileID == 1)
                {
                    // wall color
                    if (!inRoom1)
                        SDL_SetRenderDrawColor(m_renderer, 40, 60, 100, 255); // room0 wall
                    else
                        SDL_SetRenderDrawColor(m_renderer, 70, 80, 140, 255); // room1 tint
                }
                else
                {
                    // floor color
                    if (!inRoom1)
                        SDL_SetRenderDrawColor(m_renderer, 20, 20, 28, 255); // room0 floor
                    else
                        SDL_SetRenderDrawColor(m_renderer, 32, 28, 40, 255); // room1 floor tint
                }

                SDL_RenderFillRect(m_renderer, &r);
            }
        }

        // draw enemy if alive
        if (m_enemy.hp > 0)
        {
            SDL_Rect e{
                static_cast<int>(m_enemy.x) - view.x + offsetX,
                static_cast<int>(m_enemy.y) - view.y + offsetY,
                game::Enemy::WIDTH,
                game::Enemy::HEIGHT};
            SDL_SetRenderDrawColor(m_renderer, 180, 40, 40, 255);
            SDL_RenderFillRect(m_renderer, &e);
        }

        // draw active attack hitboxes
        for (auto &atk : m_attacks)
        {
            SDL_Rect r{
                atk.rect.x - view.x + offsetX,
                atk.rect.y - view.y + offsetY,
                atk.rect.w,
                atk.rect.h};
            SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 180);
            SDL_RenderFillRect(m_renderer, &r);
        }

        // draw player
        SDL_Rect p{
            static_cast<int>(m_player.x) - view.x + offsetX,
            static_cast<int>(m_player.y) - view.y + offsetY,
            game::Player::WIDTH,
            game::Player::HEIGHT};
        SDL_SetRenderDrawColor(m_renderer, 200, 220, 64, 255);
        SDL_RenderFillRect(m_renderer, &p);

        SDL_RenderPresent(m_renderer);
    }

    void Engine::capFrameRate(uint32_t frameStartMs)
    {
        uint32_t frameTimeMs = SDL_GetTicks() - frameStartMs;
        if (frameTimeMs < FRAME_MS_CAP)
            SDL_Delay(FRAME_MS_CAP - frameTimeMs);
    }
}
