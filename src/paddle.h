#ifndef PADDLE_H_
#define PADDLE_H_
#include <SDL2/SDL.h>

typedef struct {
  int vertical_position;
} paddle_t;

SDL_Rect construct_rect_from_left_paddle(paddle_t);
SDL_Rect construct_rect_from_right_paddle(paddle_t);
void move_paddle_vertically(paddle_t *, int);
#endif // PADDLE_H_
