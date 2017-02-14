#ifndef SDL_FUNC_GUARD
#define SDL_FUNC_GUARD

#define WAV_PATH "beep.wav"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

char initSDL(
      SDL_Window**
    , SDL_Renderer**
    , Mix_Chunk**
    , unsigned char
);

void drawScreen(
      SDL_Renderer**
    , SDL_Texture**
    , unsigned char[]
    , unsigned char
);

void createTile(
      SDL_Renderer**
    , SDL_Texture**
    , unsigned char
);

#endif
