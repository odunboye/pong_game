// #include "dimension.h"
#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include "util.h"
#include "pongball.h"
#include "racket.h"

struct PlayState
{
    struct Racket player, enemy;
    struct PongBall ball;
    Uint32 last_update_ms;
    struct Score score;

    // Dimmensions of the frame within which the game is played.
    const struct Dimensions *frame;
};

void playstate_init(struct PlayState *p, const struct Dimensions *frame, struct Racket player, struct Racket enemy, struct PongBall pongball);
void playstate_play(struct PlayState *p, Uint32 now_ms);
void playstate_run_collisions(struct PlayState *p, Uint32 delta_ms);
void playstate_play_movements(struct PlayState *p, Uint32 delta_ms);
void playstate_handle_event(struct PlayState *p, SDL_Event *e);
void playstate_reset(struct PlayState *p);
void playstate_render(SDL_Renderer *r, const struct PlayState *p);
void playstate_play_enemy(struct PlayState *p);
int playstate_yhits_racket(struct PongBall *ball, const struct Racket *racket);

#endif
