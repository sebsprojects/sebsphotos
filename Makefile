CC_FLAGS = -Wall -O3 -std=c99 -D_XOPEN_SOURCE=500 -I./lib/include
LN_FLAGS = -L./lib/bin -lm -lglfw -lGLESv2 -ljpeg

CC = gcc
OBJ_EXTENSION = o

# ----------------------------------------------------------------------------

SRC_FILES = $(wildcard src/*.c)
OBJ_FILES = $(addprefix bin/,$(notdir $(SRC_FILES:.c=.$(OBJ_EXTENSION))))
DEP_FILES = $(OBJ_FILES:.$(OBJ_EXTENSION)=.d)

BIN_NATIVE = ./bin/sph

native: $(BIN_NATIVE)

$(BIN_NATIVE): $(OBJ_FILES)
	$(CC) $^ $(LN_FLAGS) -o $@

-include $(DEP_FILES)

# ----------------------------------------------------------------------------

bin/%.$(OBJ_EXTENSION): src/%.c
	$(CC) $(CC_FLAGS) $(INCLUDES) -MMD -c $< -o $@

clean:
	-rm ./bin/*
