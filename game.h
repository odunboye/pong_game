#ifndef GAME_H
#define GAME_H

#include "video.h"
#include "playstate.h"

typedef struct Game
{
    struct Video video;
    PlayState play;

    void (*game_run)(struct Game *g, const char *title, Racket player, Racket enemy, PongBall pongball);
    void (*game_main)(struct Game *g);
    void (*game_check_finish_round)(struct Game *g);
    void (*game_quit)(struct Game *g);

} Game;

Game *new_game(Dimensions);
#endif
