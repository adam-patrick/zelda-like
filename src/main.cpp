#include <SDL2/SDL.h>
#include "engine/Engine.h"

int main(int, char**)
{
    zelda::engine::Engine engine;
    if (!engine.init("Milestone 6", 640, 480, false))
    {
        return 1;
    }

    engine.run();
    engine.shutdown();
    return 0;
}
