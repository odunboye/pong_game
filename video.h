
#ifndef VIDEO_H
#define VIDEO_H

#include <SDL2/SDL.h>
#include "util.h"

struct Video
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    struct Dimensions dim;
};

void video_init(struct Video *gv, const char *title);

#endif
