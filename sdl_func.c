#include "sdl_func.h"
#include "die.h"

char initSDL(
      SDL_Window **window
    , SDL_Renderer **renderer
    , Mix_Chunk **beep
    , unsigned char scale
) {
    
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        die(SDL_GetError());
        return 1;
    }

    *window = SDL_CreateWindow(
        "CHIP8",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        64 * scale,
        32 * scale,
        SDL_WINDOW_OPENGL
    );

    if (*window == NULL) {
        die(SDL_GetError());
        return 1;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL) {
        die(SDL_GetError());
        return 1;
    }

    if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        die(SDL_GetError());    
        return 1;
    }

    *beep = Mix_LoadWAV(WAV_PATH);
    if (*beep == NULL) {
        die(SDL_GetError());
	    return 1;
    }    
    return 0;
}

void createTile(
      SDL_Renderer **renderer
    , SDL_Texture **tile
    , unsigned char scale
) {
    
    *tile = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_STREAMING, scale, scale);

    // Initialize texture pixels 
    unsigned char* bytes = NULL;
    int pitch = 0;
    SDL_LockTexture(*tile, NULL, (void**)(&bytes), &pitch);
    unsigned char rgba[4] = {255, 255, 255, 255};
    for(int y = 0; y < scale; ++y) {
        for (int x = 0; x < scale; ++x) {
            memcpy(&bytes[(y * scale + x) * sizeof(rgba)], rgba, sizeof(rgba));
        }
    }
    SDL_UnlockTexture(*tile);    
}

void drawScreen(
      SDL_Renderer **renderer
    , SDL_Texture **tile
    , unsigned char gfx[]
    , unsigned char scale
) {

    // clear screen
    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);
    SDL_RenderClear(*renderer);

    SDL_Rect r;
    for(unsigned short i = 0; i < 64 * 32; i++) {
        if(gfx[i]) {
            r.x = (i & 0x3F) * scale ;
            r.y = (i >>   6) * scale;
            r.w = scale;
            r.h = scale;
	    SDL_RenderCopy(*renderer, *tile, NULL, &r);
        }
    }
    SDL_RenderPresent(*renderer);    
}



