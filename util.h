#ifndef UTIL_H
#define UTIL_H

#include <SDL2/SDL.h>

struct Score
{
    int player, enemy;
};

enum Direction
{
    LEFT = -1,
    RIGHT = 1
};

struct Dimensions
{
    int width, height;
};

enum
{
    // This can be any integer from 1 to 99, inclusive.
    END_SCORE = 30
};

enum
{
    MIDLINE_POINT_WIDTH = 3,
    MIDLINE_POINT_HEIGHT = 2 * MIDLINE_POINT_WIDTH,
    MIDLINE_POINT_MARGIN = 3,
    MIDLINE_PADDING = 20
};

enum
{
    DIGIT_PIECE_SIZE = 7,
    DIGIT_HEIGHT = 5,       // In DIGIT_PIECE_SIZE units.
    DIGIT_WIDTH = 5,        // In DIGIT_PIECE_SIZE units.
    DIGIT_INNER_MARGIN = 1, // In DIGIT_PIECE_SIZE units.
    DIGIT_OUTER_MARGIN = 2  // In DIGIT_PIECE_SIZE units.
};

struct DigitRenderingContext
{
    int x_offset;             // Offset from the midline.
    enum Direction direction; // The direction to write.

    // Dimensions within which the digit is being written. Serves to compute the
    // midline.
    const struct Dimensions *within;
};

void render_score(SDL_Renderer *r, const struct Score *s, const struct Dimensions *screen);
void render_single_score(SDL_Renderer *r,
                         int score,
                         struct DigitRenderingContext *cx);
void render_digit(SDL_Renderer *r,
                  int digit,
                  const struct DigitRenderingContext *cx);
void render_midline(SDL_Renderer *r, const struct Dimensions *screen);
int midline_npoints(const struct Dimensions *screen);
float fclamp0(float x, float max);
int is2digits(int x);
#endif
