#include <SDL2/SDL.h>
#include "engine/Engine.h"

int main(int, char**)
{
    zelda::engine::Engine engine;
    if (!engine.init("Milestone 8", 640, 480, false))
    {
        SDL_Log("Engine.init() failed");
        return 1;
    }

    engine.run();
    engine.shutdown();
    return 0;
}
