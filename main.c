#include "main.h"
#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
Mix_Chunk *ball_paddle_collision_sfx = NULL;
Mix_Chunk *ball_boundaries_collision_sfx = NULL;
Mix_Chunk *ball_score_sfx = NULL;
TTF_Font *font = NULL;

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
SDL_Rect construct_ball_rect(ball_t ball) {
  SDL_Rect ball_rect = {.x = ball.position[0],
                        .y = ball.position[1],
                        .w = BALL_RADIUS,
                        .h = BALL_RADIUS};
  return ball_rect;
}

void advance_game_state(game_state_t *game, SDL_KeyCode keycode) {
  switch (game->running_state) {
  case LEFT_PLAYER_WON:
    return;
  case RIGHT_PLAYER_WON:
    return;
  case RUNNING_STATE:
    switch (keycode) {
    case SDLK_q:
      game->running_state = QUIT_STATE;
      break;

    case SDLK_UP:
      if (game->right_player.paddle.vertical_position > WINDOW_HEIGHT)
        game->right_player.paddle.vertical_position = 0;
      else if (game->right_player.paddle.vertical_position < 0)
        game->right_player.paddle.vertical_position = WINDOW_HEIGHT;
      game->right_player.paddle.vertical_position -= PADDLE_VELOCITY;
      break;
    case SDLK_DOWN:
      if (game->right_player.paddle.vertical_position > WINDOW_HEIGHT)
        game->right_player.paddle.vertical_position = 0;
      else if (game->right_player.paddle.vertical_position < 0)
        game->right_player.paddle.vertical_position = WINDOW_HEIGHT;
      game->right_player.paddle.vertical_position += PADDLE_VELOCITY;
      break;

    case SDLK_w:
      if (game->left_player.paddle.vertical_position > WINDOW_HEIGHT)
        game->left_player.paddle.vertical_position = 0;
      else if (game->left_player.paddle.vertical_position < 0)
        game->left_player.paddle.vertical_position = WINDOW_HEIGHT;
      game->left_player.paddle.vertical_position -= PADDLE_VELOCITY;
      break;
    case SDLK_s:
      if (game->left_player.paddle.vertical_position > WINDOW_HEIGHT)
        game->left_player.paddle.vertical_position = 0;
      else if (game->left_player.paddle.vertical_position < 0)
        game->left_player.paddle.vertical_position = WINDOW_HEIGHT;
      game->left_player.paddle.vertical_position += PADDLE_VELOCITY;
      break;

    default: {
    }
    }

    game->ball.position[0] += game->ball.velocity[0];
    game->ball.position[1] += game->ball.velocity[1];

    const SDL_Rect left_paddle_rect =
        construct_rect_from_left_paddle(game->left_player.paddle);
    const SDL_Rect right_paddle_rect =
        construct_rect_from_right_paddle(game->right_player.paddle);
    const SDL_Rect ball_rect = construct_ball_rect(game->ball);

    if (SDL_HasIntersection(&right_paddle_rect, &ball_rect) ||
        SDL_HasIntersection(&left_paddle_rect, &ball_rect)) {
      game->ball.velocity[0] = -game->ball.velocity[0];
      Mix_PlayChannel(-1, ball_paddle_collision_sfx, 0);
    }

    if (game->ball.position[1] > WINDOW_HEIGHT || game->ball.position[1] < 0) {
      game->ball.velocity[1] = -game->ball.velocity[1];
      Mix_PlayChannel(-1, ball_boundaries_collision_sfx, 0);
    }

    if (game->ball.position[0] < 0) {
      game->right_player.score++;
      game->ball.position[0] = WINDOW_WIDTH / 2;
      game->ball.position[1] = rand() % WINDOW_HEIGHT;
      Mix_PlayChannel(-1, ball_score_sfx, 0);
    }
    if (game->ball.position[0] > WINDOW_WIDTH) {
      game->left_player.score++;
      game->ball.position[0] = WINDOW_WIDTH / 2;
      game->ball.position[1] = rand() % WINDOW_HEIGHT;
      Mix_PlayChannel(-1, ball_score_sfx, 0);
    }

    if (game->left_player.score >= MAX_SCORE) {
      game->running_state = LEFT_PLAYER_WON;
    }
    if (game->right_player.score >= MAX_SCORE) {
      game->running_state = RIGHT_PLAYER_WON;
    }
    break;
  default: {
  }
  }
}
void render_game(const game_state_t *game, SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);

  SDL_Rect left_paddle =
      construct_rect_from_left_paddle(game->left_player.paddle);

  SDL_Rect right_paddle =
      construct_rect_from_right_paddle(game->right_player.paddle);

  SDL_RenderFillRect(renderer, &left_paddle);
  SDL_RenderFillRect(renderer, &right_paddle);

  SDL_Rect ball_rect = construct_ball_rect(game->ball);

  SDL_RenderFillRect(renderer, &ball_rect);

  SDL_Color white = {0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE};

  char left_msg[2];
  sprintf(left_msg, "%d", game->left_player.score);
  SDL_Surface *left_msg_surface = TTF_RenderText_Solid(font, left_msg, white);
  SDL_Texture *left_msg_texture =
      SDL_CreateTextureFromSurface(renderer, left_msg_surface);
  int *lw = malloc(sizeof(int));
  int *lh = malloc(sizeof(int));

  TTF_SizeText(font, left_msg, lw, lh);
  SDL_Rect left_msg_rect = {PADDLE_HORIZONTAL_PADDING, 0, *lw, *lh};

  char right_msg[2];
  sprintf(right_msg, "%d", game->right_player.score);
  SDL_Surface *right_msg_surface = TTF_RenderText_Solid(font, right_msg, white);
  SDL_Texture *right_msg_texture =
      SDL_CreateTextureFromSurface(renderer, right_msg_surface);
  int *rw = malloc(sizeof(int));
  int *rh = malloc(sizeof(int));

  TTF_SizeText(font, right_msg, rw, rh);
  SDL_Rect right_msg_rect = {WINDOW_WIDTH - 100, 0, *rw, *rh};

  SDL_RenderCopy(renderer, left_msg_texture, NULL, &left_msg_rect);
  SDL_RenderCopy(renderer, right_msg_texture, NULL, &right_msg_rect);

  SDL_RenderPresent(renderer);

  free(lw);
  free(lh);

  free(rw);
  free(rh);
}

void init() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    fprintf(stderr, "ERROR: Could not initialize SDL: %s", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  if (Mix_Init(MIX_INIT_MP3) == 0) {
    fprintf(stderr, "ERROR: Could not initialize SDL_mixer: %s",
            SDL_GetError());
    exit(EXIT_FAILURE);
  }

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    fprintf(stderr, "ERROR: Could not open audio device: %s", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  if (TTF_Init() < 0) {
    fprintf(stderr, "ERROR: Could not initialize SDL2_TTF: %s", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  window =
      SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

  if (window == NULL) {
    clean();
    fprintf(stderr, "ERROR: Could not create the main window: %s",
            SDL_GetError());
    exit(EXIT_FAILURE);
  }

  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (renderer == NULL) {
    clean();
    fprintf(stderr, "ERROR: Could not create a renderer: %s", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  font = TTF_OpenFont("./assets/digital-7.ttf", 100);
  if (font == NULL) {
    clean();
    fprintf(stderr, "ERROR: Could not open a font: %s", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  ball_boundaries_collision_sfx = Mix_LoadWAV("./assets/wall.wav");
  if (ball_boundaries_collision_sfx == NULL) {
    clean();
    fprintf(stderr,
            "ERROR: Could not load ball collision with boundaries sfx: %s",
            SDL_GetError());
    exit(EXIT_FAILURE);
  }

  ball_paddle_collision_sfx = Mix_LoadWAV("./assets/paddle.wav");
  if (ball_paddle_collision_sfx == NULL) {
    clean();
    fprintf(stderr, "ERROR: Could not load ball collision with paddle sfx: %s",
            SDL_GetError());
    exit(EXIT_FAILURE);
  }

  ball_score_sfx = Mix_LoadWAV("./assets/score.wav");
  if (ball_score_sfx == NULL) {
    clean();
    fprintf(stderr, "ERROR: Could not load ball score sfx: %s", SDL_GetError());
    exit(EXIT_FAILURE);
  }
}
void event_loop() {
  SDL_Event event;
  game_state_t initial_game_state = {
      .left_player = {.paddle =
                          {
                              .vertical_position = WINDOW_HEIGHT / 2.0,
                          },
                      .score = 0},
      .right_player = {.paddle =
                           {
                               .vertical_position = WINDOW_HEIGHT / 2.0,
                           },
                       .score = 0},
      .ball = {.position = {WINDOW_WIDTH / 2.0, WINDOW_HEIGHT / 2.0},
               .velocity = {-4, 1}},
      .running_state = RUNNING_STATE};

  while (initial_game_state.running_state != QUIT_STATE) {
    SDL_KeyCode key = SDLK_DOLLAR;

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        initial_game_state.running_state = QUIT_STATE;
        break;
      case SDL_KEYDOWN:
        key = event.key.keysym.sym;
        break;
      default: {
      }
      }
    }
    advance_game_state(&initial_game_state, key);

    printf("left player's score: %d\n", initial_game_state.left_player.score);
    printf("right player's score: %d\n", initial_game_state.right_player.score);

    render_game(&initial_game_state, renderer);
  }
  char *winner = "no winner";

  if (initial_game_state.running_state == LEFT_PLAYER_WON)
    winner = "left";
  else if (initial_game_state.running_state == RIGHT_PLAYER_WON)
    winner = "right";

  printf("Winner is: %s", winner);
}
void clean() {
  Mix_FreeChunk(ball_boundaries_collision_sfx);
  Mix_FreeChunk(ball_paddle_collision_sfx);
  Mix_FreeChunk(ball_score_sfx);

  ball_boundaries_collision_sfx = NULL;
  ball_paddle_collision_sfx = NULL;
  ball_score_sfx = NULL;

  SDL_DestroyRenderer(renderer);
  renderer = NULL;

  SDL_DestroyWindow(window);
  window = NULL;
}
void quit() {
  Mix_Quit();
  SDL_Quit();
}

int main(void) {
  init();
  event_loop();
  clean();
  quit();

  return 0;
}
