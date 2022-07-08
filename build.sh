#!/usr/bin/env bash

set -xe

CFLAGS="-Wall -Wextra -Werror -Wno-missing-braces -std=c17 -pedantic -ggdb `pkg-config --cflags sdl2 SDL2_ttf SDL2_mixer`"
LIBS="`pkg-config --libs sdl2 SDL2_ttf SDL2_mixer` -lm"

cc $CFLAGS -o bin/pong main.c $LIBS
