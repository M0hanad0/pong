#ifndef GAME_H_
#define GAME_H_

#include "../ball.h"
#include "../paddle.h"
#include "player.h"

typedef enum {
  RUNNING_STATE,
  LEFT_PLAYER_WON,
  RIGHT_PLAYER_WON,
  QUIT_STATE
} running_state_t;

typedef struct {
  player_t left_player;
  player_t right_player;
  ball_t ball;
  running_state_t running_state;
} game_state_t;

#endif // GAME_H_
