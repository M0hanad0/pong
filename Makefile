##
# Pong
#
# @file
# @version 0.1

CC := cc
CFLAGS := -Wall -Wextra -Werror -Wno-missing-braces -std=c17 -pedantic -ggdb $(shell pkg-config --cflags sdl2 SDL2_ttf SDL2_mixer)
LIBS := $(shell pkg-config --libs sdl2 SDL2_ttf SDL2_mixer) -lm
CFILES=$(shell ls *.h)


all: clean pong
clean:
	rm -f bin/pong
	rm -f obj/*.o

pong: main.o ball.o paddle.o
	$(CC) $(CFLAGS) -o bin/pong obj/main.o obj/ball.o obj/paddle.o $(LIBS)

main.o: src/main.c src/main.h
	$(CC) -c src/main.c -o obj/main.o

ball.o: src/ball.c src/ball.h
	$(CC) -c src/ball.c -o obj/ball.o

paddle.o: src/paddle.c src/paddle.h
	$(CC) -c src/paddle.c -o obj/paddle.o

# end
