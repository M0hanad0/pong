#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>

#ifndef MAIN_H_
#define MAIN_H_

#include "game.h"
void init();
void clean();
void event_loop();
void quit();

void advance_game_state(game_state_t *, SDL_KeyCode);
void render_game_state(const game_state_t *, SDL_Renderer *);

SDL_Rect construct_rect_from_left_paddle(paddle_t);
SDL_Rect construct_rect_from_right_paddle(paddle_t);
SDL_Rect construct_ball_rect(ball_t);

#endif // MAIN_H_
