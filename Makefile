##
# Pong
#
# @file
# @version 0.1

OPTIMIZATION_LEVEL := -O3
CFLAGS := -Wall -Wextra -Werror -Wno-missing-braces -std=c17 -pedantic $(OPTIMIZATION_LEVEL) -ggdb $(shell pkg-config --cflags sdl2 SDL2_ttf SDL2_mixer)
LIBS := $(shell pkg-config --libs sdl2 SDL2_ttf SDL2_mixer) -lm

CFILES := $(shell ls src/*.c)
OBJS := $(patsubst src/%.c, obj/%.o, $(CFILES))

BINARY := bin/pong

all: $(BINARY)

clean:
	rm -f $(BINARY) $(OBJS)

$(BINARY): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $^ -c -o $@
# end
