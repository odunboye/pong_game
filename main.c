#include <SDL2/SDL.h>

#include "game.h"
#include "error_constants.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    const char *TITLE = "PONG";

    struct Game game;
    game.video.dim = (struct Dimensions){
        .width = 640,
        .height = 480};

    game_run(&game, TITLE);
    if (errorFn)
    {
        fprintf(stderr, "Error: %s\n", errorFn());
        return 1;
    }
    return 0;
}

// clang main.c game.c video.c error_constants.c playstate.c pongball.c racket.c util.c - lSDL2
