#include <stdlib.h>
#include <math.h>

#include "pongball.h"
#include "racket.h"
#include "util.h"

const float RACKET_HITBACK_MAXANGLE = 85.0f * M_PI / 180.0f;

void pongball_reset(struct PongBall *ball)
{
    ball->x = ball->within->width / 2 - (ball->size) / 2.0f;
    ball->y = ball->within->height / 2 - (ball->size) / 2.0f;
    pongball_toggle(ball, RACKET_HITBACK_MAXANGLE);
}

void pongball_score(struct PongBall *ball, int *benefit)
{
    ++*benefit;
    pongball_reset(ball);
}

void pongball_render(SDL_Renderer *r, const struct PongBall *ball)
{
    SDL_Rect ball_rect;

    ball_rect = (SDL_Rect){
        .x = ball->x,
        .y = ball->y,
        .w = ball->size,
        .h = ball->size};
    SDL_RenderFillRect(r, &ball_rect);
}

void pongball_move(struct PongBall *ball, Uint32 delta_ms)
{
    ball->x += ball->dx * delta_ms * ball->speed_ms;
    ball->y += ball->dy * delta_ms * ball->speed_ms;
    ball->x = fclamp0(ball->x, ball->within->width);
    ball->y = fclamp0(ball->y, ball->within->height - ball->size);
}

void pongball_toggle(struct PongBall *ball, const float max_angle)
{
    float angle;

    angle = rand() / (float)RAND_MAX * max_angle * 2.0f -
            max_angle;
    ball->dy = sinf(angle);
    ball->dx = ball->dx < 0 ? cosf(angle) : -cosf(angle);
}
