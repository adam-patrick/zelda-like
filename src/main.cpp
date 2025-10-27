#include "engine/Engine.h"

int main()
{
    constexpr int WINDOW_WIDTH  = 800;
    constexpr int WINDOW_HEIGHT = 480;
    constexpr bool FULLSCREEN   = false;

    zelda::engine::Engine engine;
    if (!engine.init("Zelda-Like Prototype", WINDOW_WIDTH, WINDOW_HEIGHT, FULLSCREEN))
        return 1;

    engine.run();
    engine.shutdown();
    return 0;
}
