#ifndef GAME_H_
#define GAME_H_

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define MAX_SCORE 10

#define PADDLE_WIDTH 5
#define PADDLE_HEIGHT 30
#define PADDLE_HORIZONTAL_PADDING WINDOW_WIDTH / 10
#define PADDLE_VELOCITY 30

#define BALL_RADIUS 7
#define BALL_VELOCITY 7

typedef enum {
  RUNNING_STATE,
  LEFT_PLAYER_WON,
  RIGHT_PLAYER_WON,
  QUIT_STATE
} running_state_t;

typedef struct {
  int vertical_position;
} paddle_t;

typedef struct {
  int position[2];
  float velocity[2];
} ball_t;

typedef struct {
  paddle_t paddle;
  int score;
} player_t;

typedef struct {
  player_t left_player;
  player_t right_player;
  ball_t ball;
  running_state_t running_state;
} game_t;

#endif // GAME_H_
