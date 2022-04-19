# export LD_LIBRARY_PATH=<enet path>:$LD_LIBRARY_PATH -> in terminal
OBJS = main.cpp

CC = g++

COMPILER_FLAGS = -w -fpermissive

LINKER_FLAGS = -lSDL2 -lSDL2_ttf -lSDL2_mixer

OBJ_NAME = main

all: $(OBJS) utils.hpp
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)