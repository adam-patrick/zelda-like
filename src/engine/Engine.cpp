#include "Engine.h"

#include <SDL2/SDL_image.h>

using namespace zelda;

namespace zelda::engine
{
    Engine::Engine() {}
    Engine::~Engine() { shutdown(); }

    bool Engine::init(const char *title, int windowWidth, int windowHeight, bool fullscreen)
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0)
            return false;

        // Init SDL_image for PNG loading
        int imgFlags = IMG_INIT_PNG;
        if ((IMG_Init(imgFlags) & imgFlags) != imgFlags)
        {
            SDL_Log("IMG_Init failed: %s", IMG_GetError());
            return false;
        }

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
        {
            SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
            return false;
        }

        m_renderer = SDL_CreateRenderer(
            m_window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!m_renderer)
        {
            SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
            return false;
        }

        m_windowWidth = windowWidth;
        m_windowHeight = windowHeight;

        // camera starts same size as window
        m_camera.width = windowWidth;
        m_camera.height = windowHeight;

        // player spawn
        m_player.x = 64.0f;
        m_player.y = 64.0f;

        // enemy placeholder
        m_enemy.x = 128.0f;
        m_enemy.y = 96.0f;
        m_enemy.hp = 3;

        // create our 2x2 grid of rooms (each room is 10x8 tiles)
        m_rooms.debugInitRooms(/*w=*/10, /*h=*/8);

        // Load textures
        // tiles.png: floor (0..15 x), wall(16..31 x), player(32..47 x)
        // player.png optional, but we support both keys
        if (!m_textures.loadTexture("tiles", "assets/tiles.png", m_renderer))
        {
            SDL_Log("Failed to load tiles texture");
            return false;
        }

        // Optional separate player.png. If you didn't create one, we won't fail.
        if (!m_textures.loadTexture("player", "assets/player.png", m_renderer))
        {
            SDL_Log("Player texture missing, falling back to tiles strip");
            // not fatal
        }

        // sync camera to current room so camera math is valid
        {
            game::TileMap &map = m_rooms.currentMap();
            int mapWidthPx = map.width() * game::TileMap::TILE_SIZE;
            int mapHeightPx = map.height() * game::TileMap::TILE_SIZE;
            m_camera.follow(m_player.x, m_player.y, mapWidthPx, mapHeightPx);
        }

        m_lastTickMs = SDL_GetTicks();
        m_accumulatorSec = 0.0f;
        m_running = true;
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
        // free textures before renderer goes away
        m_textures.clear();

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

        IMG_Quit();
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
        m_inputUp = keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W];
        m_inputDown = keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S];
        m_inputLeft = keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A];
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
        // input -> player intent
        m_player.moveUp = m_inputUp;
        m_player.moveDown = m_inputDown;
        m_player.moveLeft = m_inputLeft;
        m_player.moveRight = m_inputRight;

        // movement + collision
        movePlayerWithCollision(TARGET_DT_SEC);

        // room-to-room transitions
        handleRoomTransition();

        // attack cooldown
        if (m_player.attackCooldown > 0.0f)
        {
            m_player.attackCooldown -= TARGET_DT_SEC;
            if (m_player.attackCooldown <= 0.0f)
                m_player.attacking = false;
        }

        // camera follow & clamp to room
        {
            game::TileMap &map = m_rooms.currentMap();
            int mapWidthPx = map.width() * game::TileMap::TILE_SIZE;
            int mapHeightPx = map.height() * game::TileMap::TILE_SIZE;
            m_camera.follow(m_player.x, m_player.y, mapWidthPx, mapHeightPx);
        }

        // attacks + combat
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
        const int mapPixW = map.width() * tileSize;
        const int mapPixH = map.height() * tileSize;

        // Spawn points after walking through a door.
        const float doorwayCenterX = 7 * tileSize + 4.0f;
        const float topEntranceY = tileSize * 2.0f;
        const float bottomEntranceY = mapPixH - tileSize * 2.0f;

        const float leftEntranceX = tileSize * 2.0f;
        const float rightEntranceX = mapPixW - tileSize * 2.0f;
        const float midY = 3 * tileSize + 4.0f;

        SDL_Rect playerRect{
            static_cast<int>(m_player.x),
            static_cast<int>(m_player.y),
            game::Player::WIDTH,
            game::Player::HEIGHT};

        SDL_Rect northDoorTrigger{
            6 * tileSize,
            -8,
            3 * tileSize,
            16};

        SDL_Rect southDoorTrigger{
            6 * tileSize,
            mapPixH - tileSize,
            3 * tileSize,
            tileSize + 8};

        SDL_Rect westDoorTrigger{
            -8,
            3 * tileSize,
            16,
            2 * tileSize};

        SDL_Rect eastDoorTrigger{
            mapPixW - tileSize,
            3 * tileSize,
            tileSize + 8,
            2 * tileSize};

        // NORTH
        if (SDL_HasIntersection(&playerRect, &northDoorTrigger))
        {
            int beforeX = m_rooms.roomX();
            int beforeY = m_rooms.roomY();
            m_rooms.goNorth();
            if (m_rooms.roomX() != beforeX || m_rooms.roomY() != beforeY)
            {
                game::TileMap &newMap = m_rooms.currentMap();
                int newMapPixH = newMap.height() * tileSize;

                float newBottomEntranceY = newMapPixH - tileSize * 2.0f;
                m_player.x = doorwayCenterX;
                m_player.y = newBottomEntranceY;

                int newMapWidthPx = newMap.width() * tileSize;
                int newMapHeightPx = newMap.height() * tileSize;
                m_camera.follow(m_player.x, m_player.y, newMapWidthPx, newMapHeightPx);
            }
            return;
        }

        // SOUTH
        if (SDL_HasIntersection(&playerRect, &southDoorTrigger))
        {
            int beforeX = m_rooms.roomX();
            int beforeY = m_rooms.roomY();
            m_rooms.goSouth();
            if (m_rooms.roomX() != beforeX || m_rooms.roomY() != beforeY)
            {
                game::TileMap &newMap = m_rooms.currentMap();
                m_player.x = doorwayCenterX;
                m_player.y = topEntranceY;

                int newMapWidthPx = newMap.width() * tileSize;
                int newMapHeightPx = newMap.height() * tileSize;
                m_camera.follow(m_player.x, m_player.y, newMapWidthPx, newMapHeightPx);
            }
            return;
        }

        // WEST
        if (SDL_HasIntersection(&playerRect, &westDoorTrigger))
        {
            int beforeX = m_rooms.roomX();
            int beforeY = m_rooms.roomY();
            m_rooms.goWest();
            if (m_rooms.roomX() != beforeX || m_rooms.roomY() != beforeY)
            {
                game::TileMap &newMap = m_rooms.currentMap();
                int newMapPixW = newMap.width() * tileSize;

                m_player.x = newMapPixW - tileSize * 2.0f; // enter from east
                m_player.y = midY;

                int newMapWidthPx = newMap.width() * tileSize;
                int newMapHeightPx = newMap.height() * tileSize;
                m_camera.follow(m_player.x, m_player.y, newMapWidthPx, newMapHeightPx);
            }
            return;
        }

        // EAST
        if (SDL_HasIntersection(&playerRect, &eastDoorTrigger))
        {
            int beforeX = m_rooms.roomX();
            int beforeY = m_rooms.roomY();
            m_rooms.goEast();
            if (m_rooms.roomX() != beforeX || m_rooms.roomY() != beforeY)
            {
                game::TileMap &newMap = m_rooms.currentMap();

                m_player.x = leftEntranceX; // enter from west
                m_player.y = midY;

                int newMapWidthPx = newMap.width() * tileSize;
                int newMapHeightPx = newMap.height() * tileSize;
                m_camera.follow(m_player.x, m_player.y, newMapWidthPx, newMapHeightPx);
            }
            return;
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
                    m_enemy.x = -1000.0f;
                    m_enemy.y = -1000.0f;
                }
                break;
            }
        }
    }

    void Engine::renderFrame()
    {
        // Clear background
        SDL_SetRenderDrawColor(m_renderer, 8, 8, 12, 255);
        SDL_RenderClear(m_renderer);

        game::TileMap &map = m_rooms.currentMap();
        SDL_Rect view = m_camera.getViewRect();

        const int tileSize = game::TileMap::TILE_SIZE;
        int mapPxW = map.width() * tileSize;
        int mapPxH = map.height() * tileSize;

        // Center the map in the view if it's smaller
        int offsetX = (mapPxW < m_camera.width) ? (m_camera.width - mapPxW) / 2 : 0;
        int offsetY = (mapPxH < m_camera.height) ? (m_camera.height - mapPxH) / 2 : 0;

        SDL_Texture *tilesTex = m_textures.get("tiles");
        SDL_Texture *playerTex = nullptr;
        if (!playerTex)
        {
            // If player.png didn't load, reuse the tiles strip.
            playerTex = tilesTex;
        }

        // Tilesheet assumptions:
        // [0,0]-[15,15]   = floor
        // [16,0]-[31,15]  = wall
        SDL_Rect srcFloor{0, 0, 16, 16};
        SDL_Rect srcWall{16, 0, 16, 16};

        // Player sprite assumptions:
        // either its own texture at 0,0
        // or on tiles.png at x=32
        SDL_Rect srcPlayer{0, 0, 16, 16};
        if (playerTex == tilesTex)
        {
            // using combined strip: third slot
            srcPlayer.x = 32;
            srcPlayer.y = 0;
            srcPlayer.w = 16;
            srcPlayer.h = 16;
        }

        // draw tilemap
        for (int ty = 0; ty < map.height(); ++ty)
        {
            for (int tx = 0; tx < map.width(); ++tx)
            {
                int tileID = map.getTileId(tx, ty);

                SDL_Rect dst{
                    tx * tileSize - view.x + offsetX,
                    ty * tileSize - view.y + offsetY,
                    tileSize,
                    tileSize};

                if (!tilesTex)
                {
                    // fallback debug colors if texture didn't load
                    if (tileID == 1)
                        SDL_SetRenderDrawColor(m_renderer, 80, 40, 40, 255);
                    else
                        SDL_SetRenderDrawColor(m_renderer, 40, 60, 80, 255);
                    SDL_RenderFillRect(m_renderer, &dst);
                }
                else
                {
                    SDL_Rect *srcRect = (tileID == 1) ? &srcWall : &srcFloor;
                    SDL_RenderCopy(m_renderer, tilesTex, srcRect, &dst);
                }
            }
        }

        // draw enemy (still a red box)
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

        // draw attack hitboxes (yellow boxes)
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

        // draw player using texture
        {
            // draw player using fallback rect color only
            {
                SDL_Rect dstPlayer{
                    static_cast<int>(m_player.x) - view.x + offsetX,
                    static_cast<int>(m_player.y) - view.y + offsetY,
                    game::Player::WIDTH,
                    game::Player::HEIGHT};

                // bright green placeholder
                SDL_SetRenderDrawColor(m_renderer, 0, 200, 0, 255);
                SDL_RenderFillRect(m_renderer, &dstPlayer);
            }

            SDL_RenderPresent(m_renderer);
        }
    }

    void Engine::capFrameRate(uint32_t frameStartMs)
    {
        uint32_t frameTimeMs = SDL_GetTicks() - frameStartMs;
        if (frameTimeMs < FRAME_MS_CAP)
            SDL_Delay(FRAME_MS_CAP - frameTimeMs);
    }
}
