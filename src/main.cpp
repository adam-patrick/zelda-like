#include "engine/Engine.h"
#include <cstdio>

int main(int /*argc*/, char* /*argv*/[])
{
    constexpr int WINDOW_WIDTH  = 800;
    constexpr int WINDOW_HEIGHT = 480;
    constexpr bool FULLSCREEN   = false;

    zelda::engine::Engine engine;

    if (!engine.init("Zelda-Like Prototype", WINDOW_WIDTH, WINDOW_HEIGHT, FULLSCREEN))
    {
        std::printf("Engine init failed.\n");
        return 1;
    }

    engine.run();
    engine.shutdown();

    return 0;
}
