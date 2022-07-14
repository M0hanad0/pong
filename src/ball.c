#include "ball.h"
#include "constants.h"

SDL_Rect construct_ball_rect(ball_t ball) {
  SDL_Rect ball_rect = {.x = ball.position[0],
                        .y = ball.position[1],
                        .w = BALL_RADIUS,
                        .h = BALL_RADIUS};
  return ball_rect;
}
