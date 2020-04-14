#ifndef GAME_H
#define GAME_H

#include "video.h"
#include "playstate.h"

typedef struct Game
{
    struct Video video;
    struct PlayState play;
} Game;

void game_run(struct Game *g, const char *title);
void game_main(struct Game *g);
void game_check_finish_round(struct Game *g);
void game_quit(struct Game *g);

#endif
