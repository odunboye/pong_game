#ifndef PONGBALL_H
#define PONGBALL_H

#include "util.h"
#include "racket.h"

typedef struct PongBall
{
    // dy and dx should be such that dy^2 + dx^2 is 1. Collectively, they form
    // the unit direction vector of the pong ball.
    float x, y, dx, dy, speed, speed_ms;
    int size;

    // Dimmensions of the frame within which the ball is.
    const Dimensions *within;
} PongBall;

void pongball_reset(struct PongBall *ball);
void pongball_render(SDL_Renderer *r, const struct PongBall *ball);
void pongball_score(struct PongBall *ball, int *benefit);
void pongball_move(struct PongBall *ball, Uint32 delta_ms);
void pongball_toggle(struct PongBall *ball, const float angle);
#endif
