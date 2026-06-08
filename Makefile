###########################################################
# Declare Makefile variables
###########################################################

CC = g++
LANG_STD = -std=c++17
COMPILER_FLAGS = -Wall -Wfatal-errors

INCLUDE_PATH = -I./libs/ \
               -I./libs/sol/ \
               -I./libs/glm/ \
               -I./libs/lua/ \
               -I/opt/homebrew/include/SDL2

SRC_FILES = ./src/*.cpp \
            ./src/Game/*.cpp \
            ./src/Logger/*.cpp \
            ./src/ECS/*.cpp \
            ./src/AssetStore/*.cpp \
            ./libs/imgui/imgui.cpp \
            ./libs/imgui/imgui_draw.cpp \
            ./libs/imgui/imgui_widgets.cpp \
            ./libs/imgui/imgui_sdl.cpp \
            ./libs/imgui/imgui_impl_sdl.cpp

LINKER_FLAGS = -L/opt/homebrew/opt/lua@5.4/lib \
               -L/opt/homebrew/lib \
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
