#ifndef BALL_H_
#define BALL_H_
#include <SDL2/SDL.h>

typedef struct {
  int position[2];
  float velocity[2];
} ball_t;

SDL_Rect construct_ball_rect(ball_t);
#endif // BALL_H_
