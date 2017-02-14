CC = gcc
CFLAGS = -Wall -std=c99
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs) -lSDL2_mixer
SRCS = main.c execute.c sdl_func.c disassemble.c die.c
MAIN = chip8


.PHONY: all
.DEFAULT: all
all: $(MAIN) disassembler.c


$(MAIN): 
	$(CC) $(CFLAGS) $(SRCS) $(SDL_CFLAGS) $(SDL_LDFLAGS) -o $(MAIN)

disassembler.c:
	$(CC) $(CFLAGS) disassembler.c disassemble.c die.c -o disassembler

