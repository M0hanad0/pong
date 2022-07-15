#ifndef PLAYER_H_
#define PLAYER_H_
#include "paddle.h"

#define MAX_SCORE 11

typedef struct {
  paddle_t paddle;
  int score;
} player_t;

#endif // PLAYER_H_
