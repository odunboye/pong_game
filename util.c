#include "assert.h"
#include <SDL2/SDL.h>

#include "util.h"

const char SCORE_DIGITS[10][DIGIT_HEIGHT][DIGIT_WIDTH + 1] = {
    {" ****",
     "*   *",
     "* * *",
     "*   *",
     "*****"},

    {"  *  ",
     " **  ",
     "  *  ",
     "  *  ",
     "*****"},

    {" ****",
     "    *",
     "*****",
     "*    ",
     "*****"},

    {"*****",
     "    *",
     " ****",
     "    *",
     "*****"},

    {"*   *",
     "*   *",
     "*****",
     "    *",
     "    *"},

    {"**** ",
     "*    ",
     "*****",
     "    *",
     "*****"},

    {"**** ",
     "*    ",
     "*****",
     "*   *",
     "*****"},

    {"*****",
     "*   *",
     "    *",
     "    *",
     "    *"},

    {"*****",
     "*   *",
     "*****",
     "*   *",
     "*****"},

    {"*****",
     "*   *",
     "*****",
     "    *",
     "    *"},
};

void render_score(SDL_Renderer *r, const struct Score *s, const struct Dimensions *screen)
{
    struct DigitRenderingContext cx = {
        .x_offset = 0,
        .direction = LEFT,
        .within = screen};
    render_single_score(r, s->player, &cx);
    cx.direction = RIGHT;
    render_single_score(r, s->enemy, &cx);
}

void render_single_score(SDL_Renderer *r,
                         int score,
                         struct DigitRenderingContext *cx)
{
    int first_offset, second_offset;

    assert(score >= 0);
    assert(score <= END_SCORE);

    first_offset = cx->direction * DIGIT_OUTER_MARGIN * DIGIT_PIECE_SIZE;
    second_offset = cx->direction *
                    DIGIT_PIECE_SIZE * (DIGIT_OUTER_MARGIN + DIGIT_INNER_MARGIN + DIGIT_WIDTH);

    if (is2digits(score))
    {
        if (cx->direction == RIGHT)
        {
            // I always wanted the opportunity to use this kind of swap. Here it is,
            // for the first time in 9 years of programming.
            first_offset = first_offset ^ second_offset;
            second_offset = first_offset ^ second_offset;
            first_offset = first_offset ^ second_offset;
        }
        cx->x_offset = first_offset;
        render_digit(r, score % 10, cx);
        cx->x_offset = second_offset;
        render_digit(r, score / 10, cx);
    }
    else
    {
        cx->x_offset = first_offset;
        render_digit(r, score, cx);
    }
}

void render_digit(SDL_Renderer *r,
                  int digit,
                  const struct DigitRenderingContext *cx)
{
    const char(*graphic)[DIGIT_HEIGHT][DIGIT_WIDTH + 1];
    SDL_Rect digit_rect;
    int i, j, sign_offset, acc_offset, midline_x;

    digit_rect.w = digit_rect.h = DIGIT_PIECE_SIZE;
    graphic = &SCORE_DIGITS[digit];
    sign_offset = cx->direction == LEFT ? 0 : DIGIT_WIDTH * DIGIT_PIECE_SIZE;
    midline_x = cx->within->width / 2;
    acc_offset = midline_x + cx->x_offset + sign_offset;

    for (i = 0; i < DIGIT_HEIGHT; i++)
    {
        for (j = 0; j < DIGIT_WIDTH; j++)
        {
            if ((*graphic)[i][DIGIT_WIDTH - j - 1] == '*')
            {
                digit_rect.y = (DIGIT_OUTER_MARGIN + i) * DIGIT_PIECE_SIZE;
                digit_rect.x = acc_offset - (j + 1) * DIGIT_PIECE_SIZE;
                SDL_RenderFillRect(r, &digit_rect);
            }
        }
    }
}

void render_midline(SDL_Renderer *r, const struct Dimensions *screen)
{
    int npoints;
    SDL_Rect mpoint;

    npoints = midline_npoints(screen);
    mpoint = (SDL_Rect){
        .x = screen->width / 2 - MIDLINE_POINT_WIDTH / 2,
        .y = MIDLINE_PADDING,
        .w = MIDLINE_POINT_WIDTH,
        .h = MIDLINE_POINT_HEIGHT};
    SDL_RenderFillRect(r, &mpoint);
    while (--npoints > 0)
    {
        mpoint.y += MIDLINE_POINT_HEIGHT + MIDLINE_POINT_MARGIN;
        SDL_RenderFillRect(r, &mpoint);
    }
}

int midline_npoints(const struct Dimensions *screen)
{
    return 1 +
           (screen->height - MIDLINE_PADDING * 2 - MIDLINE_POINT_HEIGHT) /
               (MIDLINE_POINT_MARGIN + MIDLINE_POINT_HEIGHT);
}

/**
 * Clamps x. If x < 0, then returns 0. If x > max, then returns max.
 */
float fclamp0(float x, float max)
{
    return x > max ? max : (x < 0.0f ? 0.0f : x);

    /* Note: check the folder 'mbench' for some microbenchmarks on different
   * methods for clamping. As of now, there are 4 different methods in there.
   * And this one is about 5-10% slower than the faster one (on my machine
   * according to the microbenchmarks). */
}

int is2digits(int x)
{
    return x >= 10;
}
