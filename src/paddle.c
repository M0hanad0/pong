#include "paddle.h"
#include "constants.h"
#include <SDL2/SDL.h>

SDL_Rect construct_rect_from_left_paddle(paddle_t paddle) {

  SDL_Rect left_paddle = {.x = PADDLE_HORIZONTAL_PADDING,
                          .y = paddle.vertical_position,
                          .w = PADDLE_WIDTH,
                          .h = PADDLE_HEIGHT};
  return left_paddle;
}

SDL_Rect construct_rect_from_right_paddle(paddle_t paddle) {

  SDL_Rect right_paddle = {.x = WINDOW_WIDTH - PADDLE_WIDTH -
                                PADDLE_HORIZONTAL_PADDING,
                           .y = paddle.vertical_position,
                           .w = PADDLE_WIDTH,
                           .h = PADDLE_HEIGHT};

  return right_paddle;
}

void move_paddle_vertically(paddle_t *paddle, int delta) {
  if (paddle->vertical_position > WINDOW_HEIGHT)
    paddle->vertical_position = 0;
  else if (paddle->vertical_position < 0)
    paddle->vertical_position = WINDOW_HEIGHT;
  paddle->vertical_position += delta;
}
