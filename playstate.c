// #include <SDL2/SDL.h>

#include "playstate.h"
#include "racket.h"
#include "util.h"

// The distance the enemy is willing to tolerate and wait before he starts
// to go after the ball. If the ball is less than ENEMY_WAIT_TOLERANCE pixels
// above/below its center, it'll wait; otherwise it'll pursue the ball.
const float ENEMY_WAIT_TOLERANCE = 60 / 5.0f; //RACKET_HEIGHT = RACKET_WIDTH * 3,

enum
{
    FG_R = 255,
    FG_G = 255,
    FG_B = 255,
    FG_A = 255
};

enum
{
    BG_R = 0,
    BG_G = 0,
    BG_B = 0,
    BG_A = 255
};

void playstate_init(PlayState *this, const Dimensions *frame, struct Racket player, struct Racket enemy, struct PongBall pongball)
{
    this->frame = frame;
    this->player = player;
    this->enemy = enemy;
    this->player.y = frame->height / 2.0f - player.height / 2.0f;
    this->enemy.y = frame->height / 2.0f - enemy.height / 2.0f;
    this->ball = pongball;
    this->ball.within = frame;
    pongball_reset(&this->ball);
    this->score = (struct Score){
        .player = 0,
        .enemy = 0};
}

void playstate_play(PlayState *this, Uint32 now_ms)
{
    Uint32 delta_ms;

    delta_ms = now_ms - this->last_update_ms;

    this->playstate_run_collisions(this, delta_ms);
    this->playstate_play_enemy(this);
    this->playstate_play_movements(this, delta_ms);
    this->last_update_ms = now_ms;
}
/**
 * Checks if there will be any collisions if a movement happens given the
 * current play state.
 */
void playstate_run_collisions(PlayState *this, Uint32 delta_ms)
{
    struct PongBall *ball;
    float xp, yp; // These are x prime and y prime, the next (x,y) for ball.

    ball = &this->ball;
    // xp = ball->x + ball->dx * delta_ms * PONG_BALL_MS_SPEED;
    xp = ball->x + ball->dx * delta_ms * ball->speed_ms;
    // yp = ball->y + ball->dy * delta_ms * PONG_BALL_MS_SPEED;
    yp = ball->y + ball->dy * delta_ms * ball->speed_ms;

    // A ball can collide with the top/bottom walls, in which case its dy changes
    // sign.
    if (yp > this->frame->height - this->ball.size || yp < 0.0f)
    {
        ball->dy = -ball->dy;
    }

    // If a ball reaches the region before any racket...
    if (xp < this->player.width)
    { // player
        if (!this->playstate_yhits_racket(ball, &this->player))
        {
            pongball_score(ball, &this->score.enemy);
        }
    }
    else if (xp > this->frame->width - this->enemy.width - this->ball.size)
    { // enemy
        if (!this->playstate_yhits_racket(ball, &this->enemy))
        {
            pongball_score(ball, &this->score.player);
        }
    }
}

void playstate_play_movements(PlayState *this, Uint32 delta_ms)
{
    racket_move(&this->player, delta_ms, this->frame->height - this->player.height);
    racket_move(&this->enemy, delta_ms, this->frame->height - this->enemy.height);
    pongball_move(&this->ball, delta_ms);
}

void playstate_handle_event(PlayState *this, SDL_Event *e)
{
    if (e->type == SDL_KEYDOWN)
    {
        switch (e->key.keysym.sym)
        {
        case SDLK_UP:
            this->player.dy = -1;
            return;
        case SDLK_DOWN:
            this->player.dy = 1;
            return;
        }
    }
    this->player.dy = 0;
}

void playstate_reset(PlayState *this)
{
    this->last_update_ms = SDL_GetTicks();
    playstate_init(this, this->frame, this->player, this->enemy, this->ball);
}

void playstate_render(SDL_Renderer *r, const PlayState *this)
{
    SDL_SetRenderDrawColor(r, BG_R, BG_G, BG_B, BG_A);
    SDL_RenderClear(r);
    SDL_SetRenderDrawColor(r, FG_R, FG_G, FG_B, FG_A);
    racket_render(r, 0, this->player.y, this->player.width, this->player.height);
    racket_render(r, this->frame->width - this->enemy.width, this->enemy.y, this->enemy.width, this->enemy.height);
    pongball_render(r, &this->ball);
    render_midline(r, this->frame);
    render_score(r, &this->score, this->frame);
    SDL_RenderPresent(r);
}

void playstate_play_enemy(PlayState *this)
{
    float pong_middle_y, middle_y, diff, abs_diff;

    middle_y = this->enemy.y + this->enemy.height / 2.0f;
    pong_middle_y = this->ball.y + (this->ball.size) / 2.0f;
    diff = middle_y - pong_middle_y;
    abs_diff = fabsf(diff);
    if (abs_diff <= ENEMY_WAIT_TOLERANCE)
    {
        this->enemy.dy = 0;
    }
    else
    {
        this->enemy.dy = -diff / abs_diff;
    }
}

/**
 * This function does both the checking for racket/ball collision and the
 * update of the ball state if the collision indeed happened.
 *
 * @return 1 for if collision happened and 0 otherwise.
 */
int playstate_yhits_racket(struct PongBall *ball, const struct Racket *racket)
{
    float by0, by1, mby;
    float ry0, ry1, mry;
    float mid_distance, angle;
    int happened;

    by0 = ball->y;
    by1 = ball->y + ball->size;
    ry0 = racket->y;
    ry1 = racket->y + racket->height;

    happened = (ry0 < by0 && by0 < ry1) || (ry0 < by1 && by1 < ry1);
    if (!happened)
    {
        return 0;
    }

    // Generates an angle from -50deg to 50deg depending on where the ball hit
    // the racket. If in the upmost point then +50deg; if in the downmost point
    // then -50deg; 0deg if in the middle.
    //
    // The actual min/max angles are named constants.

    mby = by0 + (ball->size) / 2.0f;
    mry = ry0 + racket->height / 2.0f;
    mid_distance = mry - mby;
    // angle = RACKET_HITBACK_MAXANGLE * (mid_distance / MAX_HIT_DISTANCE);
    angle = racket->hitback_max_angle * (mid_distance / ((racket->height) / 2.0f + (ball->size) / 2.0f));

    ball->dy = -sinf(angle); // Y increases as you go down, not up.
    ball->dx = ball->dx < 0 ? cosf(angle) : -cosf(angle);

    return 1;
}

PlayState new_playState()
{
    PlayState this = {.playstate_init = playstate_init,
                      .playstate_play = playstate_play,
                      .playstate_run_collisions = playstate_run_collisions,
                      .playstate_play_movements = playstate_play_movements,
                      .playstate_handle_event = playstate_handle_event,
                      .playstate_reset = playstate_reset,
                      .playstate_render = playstate_render,
                      .playstate_play_enemy = playstate_play_enemy,
                      .playstate_yhits_racket = playstate_yhits_racket};

    return this;
}
