#include <SDL2/SDL.h>
#include <assert.h>

#include "game.h"

#include "error_constants.h"
#include "racket.h"
#include "pongball.h"
#include "util.h"

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

void game_run(struct Game *g, const char *title)
{

    video_init(&g->video, title);
    if (errorFn)
    {
        return;
    }

    struct Racket player = {.y = 0, .dy = 0, .width = RACKET_WIDTH, .height = RACKET_HEIGHT, .speed = RACKET_SPEED, .speed_ms = RACKET_MS_SPEED, .hitback_max_angle = 85.0f * M_PI / 180.0f};
    struct Racket enemy = {.y = 0, .dy = 0, .width = RACKET_WIDTH, .height = RACKET_HEIGHT, .speed = RACKET_SPEED, .speed_ms = RACKET_MS_SPEED, .hitback_max_angle = 85.0f * M_PI / 180.0f};
    struct PongBall ball = {.x = 0.0, .y = 0.0, .dx = 0.0, .dy = 0.0, .speed = PONG_BALL_SPEED, .speed_ms = PONG_BALL_MS_SPEED, .size = PONG_BALL_SIZE};
    playstate_init(&g->play, &g->video.dim, player, enemy, ball);
    g->game_main(g);
    g->game_quit(g);
}

void game_main(struct Game *g)
{
    enum
    {
        FPS = 60,
        SEC = 1000,
        MAX_WAIT_MS = SEC / FPS
    };

    SDL_Event e;
    Uint32 last_ms, delta_ms;

    g->play.last_update_ms = SDL_GetTicks();
    for (;;)
    {
        last_ms = SDL_GetTicks();
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                return;
            }
            playstate_handle_event(&g->play, &e);
        }
        playstate_play(&g->play, SDL_GetTicks());
        playstate_render(g->video.renderer, &g->play);
        delta_ms = SDL_GetTicks() - last_ms;
        if (delta_ms < MAX_WAIT_MS)
        {
            SDL_Delay(MAX_WAIT_MS - delta_ms);
        }
        g->game_check_finish_round(g);
    }
}

void game_check_finish_round(struct Game *g)
{
    struct Score *s;

    s = &g->play.score;
    if (s->player >= END_SCORE || s->enemy >= END_SCORE)
    {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_INFORMATION,
            "End Round",
            s->player >= END_SCORE ? "You won! Go another round." : "You lost. Try again.",
            g->video.window);
        playstate_reset(&g->play);
    }
}

void game_quit(struct Game *g)
{
    SDL_DestroyRenderer(g->video.renderer);
    SDL_DestroyWindow(g->video.window);
    SDL_Quit();
}

Game *new_game(struct Dimensions dim)
{
    Game *game = calloc(sizeof(*game), 1);

    game->game_run = game_run;
    game->game_main = game_main;
    game->game_check_finish_round = game_check_finish_round;
    game->game_quit = game_quit;

    game->video.dim = dim;

    return game;
}
