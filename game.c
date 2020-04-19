#include <SDL2/SDL.h>
#include <assert.h>

#include "game.h"

#include "error_constants.h"
#include "racket.h"
#include "pongball.h"
#include "util.h"

void game_run(Game *this, const char *title, Racket player, Racket enemy, PongBall pongball)
{
    video_init(&this->video, title);
    if (errorFn)
    {
        return;
    }

    this->playState->playstate_init(this->playState, &this->video.dim, player, enemy, pongball);
    this->game_main(this);
    this->game_quit(this);
}

void game_main(Game *this)
{
    enum
    {
        FPS = 60,
        SEC = 1000,
        MAX_WAIT_MS = SEC / FPS
    };

    SDL_Event e;
    Uint32 last_ms, delta_ms;

    this->playState->last_update_ms = SDL_GetTicks();
    for (;;)
    {
        last_ms = SDL_GetTicks();
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                return;
            }
            this->playState->playstate_handle_event(this->playState, &e);
        }
        this->playState->playstate_play(this->playState, SDL_GetTicks());
        this->playState->playstate_render(this->video.renderer, this->playState);
        delta_ms = SDL_GetTicks() - last_ms;
        if (delta_ms < MAX_WAIT_MS)
        {
            SDL_Delay(MAX_WAIT_MS - delta_ms);
        }
        this->game_check_finish_round(this);
    }
}

void game_check_finish_round(Game *this)
{
    struct Score *s;

    s = &this->playState->score;
    if (s->player >= END_SCORE || s->enemy >= END_SCORE)
    {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_INFORMATION,
            "End Round",
            s->player >= END_SCORE ? "You won! Go another round." : "You lost. Try again.",
            this->video.window);
        this->playState->playstate_reset(this->playState);
    }
}

void game_quit(Game *this)
{
    SDL_DestroyRenderer(this->video.renderer);
    SDL_DestroyWindow(this->video.window);
    SDL_Quit();
}

Game *new_game(Dimensions dim)
{
    Game *game = calloc(sizeof(*game), 1);

    game->game_run = game_run;
    game->game_main = game_main;
    game->game_check_finish_round = game_check_finish_round;
    game->game_quit = game_quit;

    game->video.dim = dim;

    game->playState = new_playState();

    return game;
}
