#include <time.h>
#include "die.h"
#include "execute.h"
#include "sdl_func.h"

void readFileIntoMemory(char *, unsigned char []);
Uint32 eventCallback(Uint32, void *);

int main (int argc, char *argv[]) {
    
    if (argc < 2) die("USAGE: chip8 <filename>");

    unsigned char fontset[80] = { 
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // initialize hardware    
    CHIP8 CH8;

    CH8.pc = 0x200;
    CH8.I  = 0;
    CH8.sp = 0;
    CH8.delay_timer = 0;
    CH8.sound_timer = 0;
    
    for (short i = 0; i < 4096;    i++) CH8.memory[i] = 0;
    for (short i = 0; i <   80;    i++) CH8.memory[i] = fontset[i];
    for (short i = 0; i <   16;    i++) CH8.stack[i]  = 0;
    for (short i = 0; i <   16;    i++) CH8.V[i]      = 0;
    for (short i = 0; i < 64 * 32; i++) CH8.gfx[i]    = 0;
    for (short i = 0; i <   16;    i++) CH8.key[i]    = 0;

    srand(time(NULL));
    readFileIntoMemory(argv[1], CH8.memory);
    
    // initialize SDL
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *tile = NULL;
    unsigned char scale = 10;
    Mix_Chunk *beep = NULL;

    initSDL(&window, &renderer, &beep, scale);
    createTile(&renderer, &tile, scale);

    // emulate cycle
    unsigned char ret;
    char step = DEBUG;
    unsigned short Hz = 500;
    unsigned short cycle = 0;
    
    SDL_Event event;
    SDL_TimerID timer;
    timer = SDL_AddTimer (17, eventCallback, NULL); // 1000/60hz ~ 17ms
    (void)timer; // -Wunused-but-set-variable
    
    do {	
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: goto QUIT;
                    case SDLK_1: CH8.key[  1] = 1; break; 
                    case SDLK_2: CH8.key[  2] = 1; break;
                    case SDLK_3: CH8.key[  3] = 1; break;
                    case SDLK_4: CH8.key[0xC] = 1; break;
                    case SDLK_q: CH8.key[  4] = 1; break;
                    case SDLK_w: CH8.key[  5] = 1; break;
                    case SDLK_e: CH8.key[  6] = 1; break;
                    case SDLK_r: CH8.key[0xD] = 1; break;
                    case SDLK_a: CH8.key[  7] = 1; break;
                    case SDLK_s: CH8.key[  8] = 1; break;
                    case SDLK_d: CH8.key[  9] = 1; break;
                    case SDLK_f: CH8.key[0xE] = 1; break;
                    case SDLK_y: CH8.key[0xA] = 1; break;
                    case SDLK_x: CH8.key[  0] = 1; break;
                    case SDLK_c: CH8.key[0xB] = 1; break;
                    case SDLK_v: CH8.key[0xF] = 1; break;
                    case SDLK_p: step = step ? 0 : 1;
                }
            }
            
            if (event.type == SDL_USEREVENT) {
                // Update timers
                if(CH8.delay_timer > 0)
                    --CH8.delay_timer;

                if(CH8.sound_timer > 0) {
                    if(CH8.sound_timer == 1)
                        if(Mix_PlayChannel(-1, beep, 0) == -1) return 1;
                    --CH8.sound_timer;
                }
            }
        }
        
        if (ret == DRAW_FLAG)
            drawScreen(&renderer, &tile, CH8.gfx,  scale);

        if (step)
            getchar();

        // clear keyboard every X cycles
        if (++cycle % Hz == 0) 
            for (short i = 0; i < 16; i++) CH8.key[i] = 0;

        SDL_Delay(1000/Hz); 
        
    } while ((ret = executeOpcode(&CH8)));

    QUIT: ;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_FreeChunk(beep);
    Mix_CloseAudio();    
    SDL_Quit();
    return 0;
}


void readFileIntoMemory(char *filename, unsigned char memory[]) {
    FILE *fp;
    unsigned char buffer[4096];
    size_t ret;

    fp = fopen(filename, "rb");
    if (fp == NULL) die("Can't open input file");

    fseek(fp, 0, SEEK_END);
    unsigned short fsize = ftell(fp);
    rewind(fp);
    
    ret = fread(buffer, 1, fsize, fp);
    if (ret < fsize) die("io read error");
    fclose(fp);

    for (short i = 0; i < fsize; i++) memory[0x200 + i] = buffer[i]; 
}

Uint32 eventCallback(Uint32 intervall, void *parameter) {
    SDL_Event event;
    SDL_UserEvent userevent;
    
    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;
    
    event.type = SDL_USEREVENT;
    event.user = userevent;
 
    SDL_PushEvent (&event);
    return intervall;
 }
