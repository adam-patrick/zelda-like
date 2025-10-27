#include "Engine.h"
#include <algorithm>

using namespace zelda;

namespace zelda::engine
{
    Engine::Engine() {}
    Engine::~Engine() { shutdown(); }

    bool Engine::init(const char* title, int windowWidth, int windowHeight, bool fullscreen)
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
        if (!m_window) return false;

        m_renderer = SDL_CreateRenderer(
            m_window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!m_renderer) return false;

        m_windowWidth  = windowWidth;
        m_windowHeight = windowHeight;

        m_player.x = 64.0f;
        m_player.y = 64.0f;

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
        if (m_renderer) { SDL_DestroyRenderer(m_renderer); m_renderer = nullptr; }
        if (m_window)   { SDL_DestroyWindow(m_window);     m_window = nullptr; }
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

        const Uint8* keys = SDL_GetKeyboardState(nullptr);
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
        m_player.moveUp    = m_inputUp;
        m_player.moveDown  = m_inputDown;
        m_player.moveLeft  = m_inputLeft;
        m_player.moveRight = m_inputRight;

        movePlayerWithCollision(TARGET_DT_SEC);

        if (m_player.attackCooldown > 0.0f)
        {
            m_player.attackCooldown -= TARGET_DT_SEC;
            if (m_player.attackCooldown <= 0.0f)
                m_player.attacking = false;
        }

        updateAttacks(TARGET_DT_SEC);
        handleCombat();
    }

    void Engine::movePlayerWithCollision(float dtSec)
    {
        SDL_FPoint vel = m_player.computeVelocity();
        float desiredDx = vel.x * dtSec;
        float desiredDy = vel.y * dtSec;

        if (desiredDx != 0.0f)
        {
            float newX = m_player.x + desiredDx;
            SDL_Rect rectX{ static_cast<int>(newX), static_cast<int>(m_player.y),
                            game::Player::WIDTH, game::Player::HEIGHT };
            if (!m_map.rectCollidesSolid(rectX))
                m_player.x = newX;
        }

        if (desiredDy != 0.0f)
        {
            float newY = m_player.y + desiredDy;
            SDL_Rect rectY{ static_cast<int>(m_player.x), static_cast<int>(newY),
                            game::Player::WIDTH, game::Player::HEIGHT };
            if (!m_map.rectCollidesSolid(rectY))
                m_player.y = newY;
        }
    }

    void Engine::spawnPlayerAttack()
    {
        const int range = 18;
        int ax = static_cast<int>(m_player.x);
        int ay = static_cast<int>(m_player.y);
        int w = 12, h = 12;

        if (m_inputUp) ay -= range;
        else if (m_inputDown) ay += game::Player::HEIGHT;
        else if (m_inputLeft) ax -= range;
        else if (m_inputRight) ax += game::Player::WIDTH;
        else ay += game::Player::HEIGHT;

        m_attacks.emplace_back(ax, ay, w, h);
    }

    void Engine::updateAttacks(float dtSec)
    {
        for (auto& atk : m_attacks)
            atk.lifetime -= dtSec;

        m_attacks.erase(
            std::remove_if(m_attacks.begin(), m_attacks.end(),
                           [](const game::PlayerAttack& a){ return a.isExpired(); }),
            m_attacks.end());
    }

    void Engine::handleCombat()
    {
        if (m_enemy.hp <= 0)
            return;

        for (auto& atk : m_attacks)
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
        SDL_SetRenderDrawColor(m_renderer, 8, 8, 12, 255);
        SDL_RenderClear(m_renderer);

        m_map.render(m_renderer);
        m_enemy.render(m_renderer);

        for (auto& atk : m_attacks)
        {
            SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 180);
            SDL_RenderFillRect(m_renderer, &atk.rect);
        }

        m_player.render(m_renderer);
        SDL_RenderPresent(m_renderer);
    }

    void Engine::capFrameRate(uint32_t frameStartMs)
    {
        uint32_t frameTimeMs = SDL_GetTicks() - frameStartMs;
        if (frameTimeMs < FRAME_MS_CAP)
            SDL_Delay(FRAME_MS_CAP - frameTimeMs);
    }
}
