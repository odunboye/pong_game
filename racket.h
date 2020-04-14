#ifndef RACKET_H
#define RACKET_H
#include <SDL2/SDL.h>
#include "pongball.h"

struct Racket
{
    // dy should be 0, 1 or -1. It's only to indicate the direction of the
    // change in y.
    float y, dy, speed_ms;
    int width, height, speed;
    int hitback_max_angle;
};

void racket_render(SDL_Renderer *r, int x, int y, int w, int h);
void racket_move(struct Racket *r, Uint32 delta_ms, float maxY);

#endif
