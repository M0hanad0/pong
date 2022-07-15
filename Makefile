##
# Pong
#
# @file
# @version 0.1

BINARY				:= pong
CODE_DIRS			:= src
LIBS 				:= sdl2 SDL2_ttf SDL2_mixer
INCLUDE_DIRS		:= -I./src/include -I./src $(shell pkg-config --cflags-only-I $(LIBS))

OPTIMIZATION_LEVEL	:= -O3
DEPENDENCY_FLAGS    := -MP -MD
LIBS_FLAGS			:= $(shell pkg-config --libs $(LIBS)) -lm
CFLAGS				:= -Wall -Wextra -Werror -Wno-missing-braces -std=c17 -pedantic -ggdb $(OPTIMIZATION_LEVEL) $(INCLUDE_DIRS) $(DEPENDENCY_FLAGS) $(LIBS_FLAGS) $(shell pkg-config --cflags-only-other $(LIBS))

CFILES				:= $(foreach D, $(CODE_DIRS), $(wildcard $(D)/*.c))
OBJECTS				:= $(CFILES:.c=.o)
DEPENDENCY_FILES	:= $(CFILES:.c=.d)

all: $(BINARY)

clean:
	rm -f $(BINARY) $(OBJECTS) $(DEPENDENCY_FILES)

package: clean
	tar vczf dist.tgz *

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) -o bin/$@ $^

%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(DEPENDENCY_FILES)
# end
