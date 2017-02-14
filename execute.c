#define VX CH8->V[(opcode & 0x0F00) >> 8]
#define VY CH8->V[(opcode & 0x00F0) >> 4]
#define GFX_PIXEL_INDEX (64 * ((VY + line) & 0x1F) + ((VX + bit_index) & 0x3F)) // wraps around both edges
#define GFX_PIXEL CH8->gfx[GFX_PIXEL_INDEX]
#define MEM_PIXEL ((CH8->memory[CH8->I + line] & mask) >> (7 - bit_index))

#define DRAW_FLAG 1
#define TRUE 1
#define FALSE 0

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "die.h"
#include "execute.h"
#include "disassemble.h"

unsigned char executeOpcode(CHIP8 *CH8) {
    
    unsigned short opcode = CH8->memory[CH8->pc] << 8 | CH8->memory[CH8->pc + 1];
    char instruction [20];
    memset(instruction, '\0', sizeof(instruction));
    unsigned char ret_code = 0xFF;
    
    switch(opcode & 0xF000) {

        case 0x0000: // Sets VX to the value of VY
            switch (opcode & 0x00FF) {
                case 0x0000:
                    printf("0NNN not implemented\n");
                    return 0;
                
                case 0x00E0: // clear display      
                    for (short i = 0; i < 64 * 32; i++) CH8->gfx[i] = 0;
                    CH8->pc += 2;
                    break;

                case 0x00EE:
                    (CH8->sp)--;
                    CH8->pc = CH8->stack[CH8->sp] + 2;
                    break;

                default:
                    printf("Unknown opcode: 0x%X\n", opcode);
            }
            break;

        case 0x1000: // Jumps to address NNN 
            CH8->pc = opcode & 0x0FFF;
            break;

        case 0x2000: // Calls subroutine at NNN
            CH8->stack[CH8->sp] = CH8->pc;
            (CH8->sp)++;
            CH8->pc = opcode & 0x0FFF;
            break;

        case 0x3000: // Skips the next instruction if VX equals NN
            if (VX == (opcode & 0x00FF)) CH8->pc += 4;
            else                         CH8->pc += 2;
            break;

        case 0x4000: // Skips the next instruction if VX doesn't equals NN
            if (VX != (opcode & 0x00FF)) CH8->pc += 4;
            else                         CH8->pc += 2;
            break;

        case 0x5000: // Skips the next instruction if VX equals VY
            if (VX == VY) CH8->pc += 4;
            else          CH8->pc += 2;
            break;

        case 0x6000: // Sets VX to NN
            VX = (opcode & 0x00FF);
            CH8->pc += 2;
            break;

        case 0x7000: // Adds NN to VX
            VX += (opcode & 0x00FF);
            CH8->pc += 2;
            break;

        case 0x8000: 
            switch (opcode & 0x000F) {
                case 0x0000: // Sets VX to the value of VY
                    VX = VY;
                    CH8->pc += 2;
                    break;

                case 0x0001: // Sets VX to VX OR VY
                    VX |= VY;
                    CH8->pc += 2;
                    break;

                case 0x0002: // Sets VX to VX AND VY
                    VX &= VY;
                    CH8->pc += 2;
                    break;

                case 0x0003: // Sets VX to VX XOR VY
                    VX ^= VY;
                    CH8->pc += 2;
                    break;

                case 0x0004: /// Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.                
                    CH8->V[0xF] = (VX + VY > 255) ? 1 : 0;
                    VX += VY;
                    CH8->pc += 2;
                    break;

                case 0x0005: // VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    CH8->V[0xF] = (VX >= VY) ? 1 : 0;
                    VX -= VY;
                    CH8->pc += 2;
                    break;

                case 0x0006: // Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
                    CH8->V[0xF] = VX & 1;
                    VX >>= 1;
                    CH8->pc += 2;
                    break;

                case 0x0007: // Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    CH8->V[0xF] = (VY >= VX) ? 1 : 0;
                    VX = VY - VX;
                    CH8->pc += 2;
                    break;

                case 0x000E: // Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
                    CH8->V[0xF] = (VX & 0x80) >> 7;
                    VX <<= 1;
                    CH8->pc += 2;
                    break;
		
                default:
                    printf ("Unknown opcode: 0x%X\n", opcode);         
            }
            break;

        case 0x9000: // Skips the next instruction if VX doesn't equal VY.
            if (VX != VY) CH8->pc += 4;
            else          CH8->pc += 2;
            break;

        case 0xA000: // ANNN: Sets I to the address NNN
            CH8->I = opcode & 0x0FFF;
            CH8->pc += 2;
            break;

        case 0xB000: // BNNN: Jump to address NNN+ V[0]
            CH8->pc = (opcode & 0x0FFF) + CH8->V[0];
            break;

        case 0xC000: // Sets VX to the result of a bitwise and operation on a random number
            VX = (rand() & 0xFF) & (opcode & 0x00FF);
            CH8->pc += 2;
            break;   

        case 0xD000:
            ;
            short mask;
            short collison = FALSE;

            for (short line = 0; line < (opcode & 0x000F); line++) {
                mask = 0x80;
                for (short bit_index = 0; bit_index < 8; bit_index++) {
                    if(GFX_PIXEL_INDEX >= 64*32) continue;
                    // flip switch from on to off is a collision
                    if (GFX_PIXEL && !(GFX_PIXEL ^ MEM_PIXEL)) collison = TRUE;
                    GFX_PIXEL ^= MEM_PIXEL; 
                    mask >>= 1;
                }
            }
	    
            CH8->V[0xF] = collison; //set VF to 1 if ___ANY___ bit got switched
            CH8->pc += 2;
            ret_code = DRAW_FLAG;
            break;

        case 0xE000:
            switch(opcode & 0x00FF) {
                case 0x009E: // Skips the next instruction if the key stored in VX is pressed
                    if (CH8->key[VX]) { 
                        CH8->pc += 4;
                        CH8->key[VX] = 0;
                    } else
                        CH8->pc += 2;
                    break;

                case 0x00A1: // Skips the next instruction if the key stored in VX isn't pressed
                    if (!CH8->key[VX]) 
                        CH8->pc += 4;
                    else {
                        CH8->pc += 2;
                        CH8->key[VX] = 0;
                    }
                    break;
                 
                default:
                    printf ("Unknown opcode: 0x%X\n", opcode);                     
            }
            break;
    
        case 0xF000: 
            switch(opcode & 0x00FF) {
                case 0x0007: // Sets VX to the value of the delay timer
                    VX = CH8->delay_timer;
                    CH8->pc += 2;
                    break;

                case 0x0015: // Sets the delay timer to VX
                    CH8->delay_timer = VX;
                    CH8->pc += 2;
                    break;

                case 0x0018: // Sets the sound timer to VX
                    CH8->sound_timer = VX;
                    CH8->pc += 2;
                    break;

                case 0x0029: // Sets I to the location of the sprite for the character in VX
                    if (VX <= 0xFF) CH8->I = VX * 5;
                    CH8->pc += 2;
                    break;

                case 0x0033: // BCD, e.g. 245 -> *(I+0) = 2, *(I+1) = 4, *(I+2) = 5
                    ;
                    unsigned char count  = 0;
                    unsigned char number = VX;
                    
                    while (number - 100 >= 0) { number -= 100; count++; }
                    CH8->memory[CH8->I  ] = count; count = 0;
                    while (number - 10  >= 0) { number -=  10; count++; }            
                    CH8->memory[CH8->I+1] = count;          
                    CH8->memory[CH8->I+2] = number;
                    CH8->pc += 2;
                    break;

                case 0x000A: // Wait for a key press, store the value of the key in Vx.
                    for (short i = 0; i < 16; i++) {
                        if (CH8->key[i]) {
                            VX = i;
                            CH8->key[i] = 0;
                            CH8->pc += 2;
                            break;
                        }
                    }
                    break;
		    
                case 0x001E:  // Adds VX to I
                    CH8->V[0xF] = (CH8->I + VX > 0x0FFF) ? 1 : 0; // undocumented
                    CH8->I += VX;
                    CH8->pc += 2;
                    break;

                case 0x0055: // Stores V0 to VX (including VX) in memory starting at address I
                    for (unsigned char i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
                        CH8->memory[CH8->I + i] = CH8->V[i];
                    }
                    //CH8->I += ((opcode & 0x0F00) >> 8) + 1; // QUIRK !!!		    
                    CH8->pc += 2;
                    break;
                    
                case 0x0065: // Fills V0 to VX (including VX) with values from memory starting at address I
                    for (unsigned char i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {                     
                        CH8->V[i] = CH8->memory[CH8->I + i];
                    }
                    //CH8->I += ((opcode & 0x0F00) >> 8) + 1; // QUIRK !!!
                    CH8->pc += 2;
                    break;

                default:
                    printf("Unknown opcode: 0x%X\n", opcode);
            }
            break;
     
        default:
            printf("Unknown opcode: 0x%X\n", opcode);
    }

    if(DEBUG) {
        printf("INSTR \t%s\n", disassembleOpcode(opcode, instruction));
        printf("OPCODE \t0x%04X\n", opcode);
        printf("PC \t0x%04X\n", CH8->pc);
        printf("I  \t0x%04X\n", CH8->I);
        printf("SP \t%d\n", CH8->sp);
        printf("DELAY \t0x%02X\n", CH8->delay_timer);
        printf("SOUND \t0x%02X\n", CH8->sound_timer);
        printf("\nREG\n");
        for (short i = 0; i < 16; i++) printf("V%01X ", i);
        printf("\n");        
        for (short i = 0; i < 16; i++) printf("%02X ", CH8->V[i]);
        printf("\n\n");
        printf("MEM\n0x%04X: ", CH8->I);
        for (short i = 0; i < 16; i++) {
            printf("%04X ", CH8->memory[CH8->I+i]);
            if (i == 7) printf("\n0x%04X: ", CH8->I+8);
        }
        printf("\n");
        if (CH8->sp) {
            printf("\nSTACK\n");
            for (short i = 0; i < CH8->sp; i++) printf("0x%04X ", CH8->stack[i]);
            printf("\n");
        }
        printf("\n\n\n");
    }
    
    return ret_code;
}
