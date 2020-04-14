

#include <SDL2/SDL.h>

#include "racket.h"
#include "pongball.h"
#include "util.h"

void racket_render(SDL_Renderer *r, int x, int y, int w, int h)
{
    SDL_Rect racket;

    racket = (SDL_Rect){
        .x = x,
        .y = y,
        .w = w,
        .h = h};
    SDL_RenderFillRect(r, &racket);
}

void racket_move(struct Racket *r, Uint32 delta_ms, float maxY)
{
    r->y += delta_ms * r->speed_ms * r->dy;
    r->y = fclamp0(r->y, maxY);
}
