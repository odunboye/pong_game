#include <SDL2/SDL.h>

#include "game.h"
#include "error_constants.h"

/** Size values are in pixels by default, unless specified otheriwse. */
enum
{
    RACKET_WIDTH = 20,
    RACKET_HEIGHT = RACKET_WIDTH * 3,
    RACKET_SPEED = 550 /* Pixels per second. */
};

const float RACKET_MS_SPEED = RACKET_SPEED / 1000.0f;

enum
{
    PONG_BALL_SIZE = 20,
    PONG_BALL_SPEED = 11 * (RACKET_SPEED / 10)
};

const float PONG_BALL_MS_SPEED = PONG_BALL_SPEED / 1000.0f;

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    const char *TITLE = "PONG";

    Racket player = {.y = 0, .dy = 0, .width = RACKET_WIDTH, .height = RACKET_HEIGHT, .speed = RACKET_SPEED, .speed_ms = RACKET_MS_SPEED, .hitback_max_angle = 85.0f * M_PI / 180.0f};
    Racket enemy = {.y = 0, .dy = 0, .width = RACKET_WIDTH, .height = RACKET_HEIGHT, .speed = RACKET_SPEED, .speed_ms = RACKET_MS_SPEED, .hitback_max_angle = 85.0f * M_PI / 180.0f};
    PongBall ball = {.x = 0.0, .y = 0.0, .dx = 0.0, .dy = 0.0, .speed = PONG_BALL_SPEED, .speed_ms = PONG_BALL_MS_SPEED, .size = PONG_BALL_SIZE};

    Game *game = new_game((Dimensions){.width = 640, .height = 480});

    game->game_run(game, TITLE, player, enemy, ball);
    if (errorFn)
    {
        fprintf(stderr, "Error: %s\n", errorFn());
        return 1;
    }
    return 0;
}

// clang main.c game.c video.c error_constants.c playstate.c pongball.c racket.c util.c - lSDL2
