#include "main.h"
#include "include/constants.h"
#include "include/game.h"
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

#define RANDOM_INT(min, max)                                                   \
  (min) + rand() / (RAND_MAX / ((max) - (min) + 1) + 1)

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static Mix_Chunk *ball_paddle_collision_sfx = NULL;
static Mix_Chunk *ball_boundaries_collision_sfx = NULL;
static Mix_Chunk *ball_score_sfx = NULL;
static TTF_Font *font = NULL;

void handle_input(game_state_t *game, SDL_KeyCode keycode) {
  int rdelta = 0, ldelta = 0;
  switch (keycode) {
  case SDLK_q:
    game->running_state = QUIT_STATE;
    break;

  case SDLK_UP:
    rdelta = -PADDLE_VELOCITY;
    break;
  case SDLK_DOWN:
    rdelta = PADDLE_VELOCITY;
    break;

  case SDLK_w:
    ldelta = -PADDLE_VELOCITY;
    break;
  case SDLK_s:
    ldelta = PADDLE_VELOCITY;
    break;

  default: {
  }
  }
  move_paddle_vertically(&game->right_player.paddle, rdelta);
  move_paddle_vertically(&game->left_player.paddle, ldelta);
}

void advance_game_state(game_state_t *game) {
  switch (game->running_state) {
  case LEFT_PLAYER_WON:
    return;
  case RIGHT_PLAYER_WON:
    return;
  case QUIT_STATE:
    return;
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

  if (game->ball.position[1] >= WINDOW_HEIGHT || game->ball.position[1] <= 0) {
    game->ball.velocity[1] = -game->ball.velocity[1];
    Mix_PlayChannel(-1, ball_boundaries_collision_sfx, 0);
  }

  if (game->ball.position[0] < 0 || game->ball.position[0] > WINDOW_WIDTH) {
    if (game->ball.position[0] > WINDOW_WIDTH) {
      game->left_player.score++;
    } else {
      game->right_player.score++;
    }

    game->ball.position[0] = WINDOW_WIDTH / 2;
    game->ball.position[1] = rand() % WINDOW_HEIGHT;
    Mix_PlayChannel(-1, ball_score_sfx, 0);
  }

  if (game->left_player.score >= MAX_SCORE) {
    game->running_state = LEFT_PLAYER_WON;
    return;
  }
  if (game->right_player.score >= MAX_SCORE) {
    game->running_state = RIGHT_PLAYER_WON;
    return;
  }
}

void render_score(player_t player, int horizontal_position) {

  SDL_Color white = {0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE};

  char msg[2];
  sprintf(msg, "%d", player.score);
  SDL_Surface *msg_surface = TTF_RenderText_Solid(font, msg, white);
  SDL_Texture *msg_texture =
      SDL_CreateTextureFromSurface(renderer, msg_surface);
  int *w = malloc(sizeof(int));
  int *h = malloc(sizeof(int));

  TTF_SizeText(font, msg, w, h);
  SDL_Rect msg_rect = {horizontal_position, 0, *w, *h};

  SDL_RenderCopy(renderer, msg_texture, NULL, &msg_rect);

  free(w);
  free(h);
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

  render_score(game->right_player, WINDOW_WIDTH - 100);
  render_score(game->left_player, PADDLE_HORIZONTAL_PADDING);

  SDL_RenderPresent(renderer);
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

  font = TTF_OpenFont("assets/digital-7.ttf", 100);
  if (font == NULL) {
    clean();
    fprintf(stderr, "ERROR: Could not open a font: %s", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  ball_boundaries_collision_sfx = Mix_LoadWAV("assets/wall.wav");
  if (ball_boundaries_collision_sfx == NULL) {
    clean();
    fprintf(stderr,
            "ERROR: Could not load ball collision with boundaries sfx: %s",
            SDL_GetError());
    exit(EXIT_FAILURE);
  }

  ball_paddle_collision_sfx = Mix_LoadWAV("assets/paddle.wav");
  if (ball_paddle_collision_sfx == NULL) {
    clean();
    fprintf(stderr, "ERROR: Could not load ball collision with paddle sfx: %s",
            SDL_GetError());
    exit(EXIT_FAILURE);
  }

  ball_score_sfx = Mix_LoadWAV("assets/score.wav");
  if (ball_score_sfx == NULL) {
    clean();
    fprintf(stderr, "ERROR: Could not load ball score sfx: %s", SDL_GetError());
    exit(EXIT_FAILURE);
  }
}
void event_loop(game_state_t *game_state) {
  while (game_state->running_state != QUIT_STATE) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        game_state->running_state = QUIT_STATE;
        break;
      case SDL_KEYDOWN:
        handle_input(game_state, event.key.keysym.sym);
        break;
      default: {
      }
      }
    }
    advance_game_state(game_state);
    render_game(game_state, renderer);
  }
  char *winner = "no winner";

  if (game_state->running_state == LEFT_PLAYER_WON)
    winner = "left";
  else if (game_state->running_state == RIGHT_PLAYER_WON)
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
  static game_state_t initial_game_state = {
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
               .velocity = {0, 0}},
      .running_state = RUNNING_STATE};

  initial_game_state.ball.velocity[0] = RANDOM_INT(-7.5, 7.5);
  initial_game_state.ball.velocity[1] = RANDOM_INT(-7.5, 7.5);

  init();
  event_loop(&initial_game_state);
  clean();
  quit();

  return 0;
}
