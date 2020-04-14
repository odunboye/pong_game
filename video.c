#include <SDL2/SDL.h>

#include "video.h"
#include "error_constants.h"

void video_init(struct Video *gv, const char *title)
{
    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        gv->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED, gv->dim.width, gv->dim.height,
                                      SDL_WINDOW_SHOWN);
        if (gv->window)
        {
            gv->renderer = SDL_CreateRenderer(gv->window, -1,
                                              SDL_RENDERER_ACCELERATED);
            if (gv->renderer)
            {
                return;
            }
            SDL_DestroyWindow(gv->window);
        }
        SDL_Quit();
    }
    errorFn = SDL_GetError;
}

