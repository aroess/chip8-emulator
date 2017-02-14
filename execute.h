#ifndef EXECUTE_GUARD
#define EXECUTE_GUARD

#define DRAW_FLAG 1
#define DEBUG 0

typedef struct CHIP8 { 
    unsigned char  memory[4096];
    unsigned char  V[16];
    unsigned short stack[16];

    unsigned short pc;
    unsigned short I;
    unsigned short sp;

    unsigned char gfx[64 * 32];
    unsigned char key[16];
    unsigned char delay_timer;
    unsigned char sound_timer;
} CHIP8;

unsigned char executeOpcode(CHIP8 *CH8);

#endif
