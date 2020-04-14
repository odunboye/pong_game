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

void playstate_init(struct PlayState *p, const struct Dimensions *frame, struct Racket player, struct Racket enemy, struct PongBall pongball)
{
    p->frame = frame;
    p->player = player;
    p->enemy = enemy;
    p->player.y = frame->height / 2.0f - player.height / 2.0f;
    p->enemy.y = frame->height / 2.0f - enemy.height / 2.0f;
    p->ball = pongball;
    p->ball.within = frame;
    pongball_reset(&p->ball);
    p->score = (struct Score){
        .player = 0,
        .enemy = 0};
}

void playstate_play(struct PlayState *p, Uint32 now_ms)
{
    Uint32 delta_ms;

    delta_ms = now_ms - p->last_update_ms;

    playstate_run_collisions(p, delta_ms);
    playstate_play_enemy(p);
    playstate_play_movements(p, delta_ms);
    p->last_update_ms = now_ms;
}
/**
 * Checks if there will be any collisions if a movement happens given the
 * current play state.
 */
void playstate_run_collisions(struct PlayState *p, Uint32 delta_ms)
{
    struct PongBall *ball;
    float xp, yp; // These are x prime and y prime, the next (x,y) for ball.

    ball = &p->ball;
    // xp = ball->x + ball->dx * delta_ms * PONG_BALL_MS_SPEED;
    xp = ball->x + ball->dx * delta_ms * ball->speed_ms;
    // yp = ball->y + ball->dy * delta_ms * PONG_BALL_MS_SPEED;
    yp = ball->y + ball->dy * delta_ms * ball->speed_ms;

    // A ball can collide with the top/bottom walls, in which case its dy changes
    // sign.
    if (yp > p->frame->height - p->ball.size || yp < 0.0f)
    {
        ball->dy = -ball->dy;
    }

    // If a ball reaches the region before any racket...
    if (xp < p->player.width)
    { // player
        if (!playstate_yhits_racket(ball, &p->player))
        {
            pongball_score(ball, &p->score.enemy);
        }
    }
    else if (xp > p->frame->width - p->enemy.width - p->ball.size)
    { // enemy
        if (!playstate_yhits_racket(ball, &p->enemy))
        {
            pongball_score(ball, &p->score.player);
        }
    }
}

void playstate_play_movements(struct PlayState *p, Uint32 delta_ms)
{
    racket_move(&p->player, delta_ms, p->frame->height - p->player.height);
    racket_move(&p->enemy, delta_ms, p->frame->height - p->enemy.height);
    pongball_move(&p->ball, delta_ms);
}

void playstate_handle_event(struct PlayState *p, SDL_Event *e)
{
    if (e->type == SDL_KEYDOWN)
    {
        switch (e->key.keysym.sym)
        {
        case SDLK_UP:
            p->player.dy = -1;
            return;
        case SDLK_DOWN:
            p->player.dy = 1;
            return;
        }
    }
    p->player.dy = 0;
}

void playstate_reset(struct PlayState *p)
{
    p->last_update_ms = SDL_GetTicks();
    playstate_init(p, p->frame, p->player, p->enemy, p->ball);
}

void playstate_render(SDL_Renderer *r, const struct PlayState *p)
{
    SDL_SetRenderDrawColor(r, BG_R, BG_G, BG_B, BG_A);
    SDL_RenderClear(r);
    SDL_SetRenderDrawColor(r, FG_R, FG_G, FG_B, FG_A);
    racket_render(r, 0, p->player.y, p->player.width, p->player.height);
    racket_render(r, p->frame->width - p->enemy.width, p->enemy.y, p->enemy.width, p->enemy.height);
    pongball_render(r, &p->ball);
    render_midline(r, p->frame);
    render_score(r, &p->score, p->frame);
    SDL_RenderPresent(r);
}

void playstate_play_enemy(struct PlayState *p)
{
    float pong_middle_y, middle_y, diff, abs_diff;

    middle_y = p->enemy.y + p->enemy.height / 2.0f;
    pong_middle_y = p->ball.y + (p->ball.size) / 2.0f;
    diff = middle_y - pong_middle_y;
    abs_diff = fabsf(diff);
    if (abs_diff <= ENEMY_WAIT_TOLERANCE)
    {
        p->enemy.dy = 0;
    }
    else
    {
        p->enemy.dy = -diff / abs_diff;
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
