###########################################################
# Declare Makefile variables
###########################################################

CC = g++
LANG_STD = -std=c++17
COMPILER_FLAGS = -Wall -Wfatal-errors

INCLUDE_PATH = -Ilibs/glm \
               -Ilibs/imgui \
               -Ilibs/lua \
               -Ilibs/sol \
               -I/opt/homebrew/include/SDL2

SRC_FILES = ./src/*.cpp \
            ./src/Game/*.cpp \
            ./src/Logger/*.cpp \
            ./src/ECS/*.cpp \
            ./src/AssetStore/*.cpp

LINKER_FLAGS = -L/opt/homebrew/lib \
               -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer \
               -llua

OBJ_NAME = gameengine

###########################################################
# Declare Makefile rules
###########################################################

build:
	$(CC) $(LANG_STD) $(COMPILER_FLAGS) $(INCLUDE_PATH) $(SRC_FILES) -o $(OBJ_NAME) $(LINKER_FLAGS)

run:
	./$(OBJ_NAME)

clean:
	rm -f $(OBJ_NAME)
