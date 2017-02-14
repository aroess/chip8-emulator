#define X    (opcode & 0x0F00) >> 8
#define Y    (opcode & 0x00F0) >> 4
#define val  (opcode & 0x00FF)
#define addr (opcode & 0x0FFF)


#include <stdio.h>
#include <string.h>

char *disassembleOpcode(unsigned short opcode, char instruction[]) {
    
    switch(opcode & 0xF000) {

        case 0x0000: // Sets VX to the value of VY
            switch (opcode & 0x00FF) {
                case 0x0000:
                    sprintf(instruction, "SYS 0x%04X", addr);
                    break;
                    
                case 0x00E0: // clear display
                    strcpy(instruction, "CLS");
                    break;

                case 0x00EE:
                    strcpy(instruction, "RET");
                    break;

                default:
                    printf("not implemented\n");
            }
            break;

        case 0x1000: // Jumps to address NNN
            sprintf(instruction, "JUMP 0x%04X", addr);           
            break;

        case 0x2000: // Calls subroutine at NNN
            sprintf(instruction, "CALL 0x%04X", addr);
            break;

        case 0x3000: // Skips the next instruction if VX equals NN
            sprintf(instruction, "SKIP V%X, 0x%02X", X, val);            
            break;

        case 0x4000: // Skips the next instruction if VX doesn't equals NN
            sprintf(instruction, "NSKIP V%X, 0x%02X", X, val);
            break;

        case 0x5000: // Skips the next instruction if VX equals VY
            sprintf(instruction, "SKIP V%X, V%X", X, Y);
            break;

        case 0x6000: // Sets VX to NN
            sprintf(instruction, "LOAD V%X, 0x%02X", X, val);
            break;

        case 0x7000: // Adds NN to VX
            sprintf(instruction, "ADD V%X, 0x%02X", X, val);
            break;

        case 0x8000: 
            switch (opcode & 0x000F) {
                case 0x0000: // Sets VX to the value of VY
                    sprintf(instruction, "LOAD V%X, V%X", X, Y);
                    break;

                case 0x0001: // Sets VX to VX OR VY
                    sprintf(instruction, "OR V%X, V%X", X, Y);
                    break;

                case 0x0002: // Sets VX to VX AND VY
                    sprintf(instruction, "AND V%X, V%X", X, Y);
                    break;                

                case 0x0003: // Sets VX to VX XOR VY
                    sprintf(instruction, "XOR V%X, V%X", X, Y);
                    break;

                case 0x0004: /// Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
                    sprintf(instruction, "ADD V%X, V%X", X, Y);
                break;

                case 0x0005: // VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    sprintf(instruction, "SUB V%X, V%X", X, Y);
                break;

                case 0x0006: // Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
                    sprintf(instruction, "RSHIFT V%X", X);
                    break;

                case 0x0007: // Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    sprintf(instruction, "NSUB V%X V%X", X, Y);
                    break;

                case 0x000E: // Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
                    sprintf(instruction, "LSHIFT V%X", X);
                    break;

                default:
                    printf ("Unknown opcode: 0x%X\n", opcode);         
            }
            break;

        case 0x9000: // Skips the next instruction if VX doesn't equal VY.
            sprintf(instruction, "NSKIP V%X, V%X", X, Y);
            break;

        case 0xA000: // ANNN: Sets I to the address NNN
            sprintf(instruction, "LOAD I, 0x%04X", addr);
            break;

        case 0xB000: // BNNN: Jump to address NNN+ V[0]
            sprintf(instruction, "JUMP V0, 0x%04X", addr);
            break;

        case 0xC000: // Sets VX to the result of a bitwise and operation on a random number
            sprintf(instruction, "RAND V%X, 0x%02X", X, val);
            break;   

        case 0xD000:
            sprintf(instruction, "DRAW V%X, V%X, 0x%X", X, Y, val & 0xF);
            break;
            
        case 0xE000:
            switch(opcode & 0x00FF) {
                case 0x009E: // Skips the next instruction if the key stored in VX is pressed
                    sprintf(instruction, "SKEY V%X", X);
                    break;

                case 0x00A1: // Skips the next instruction if the key stored in VX isn't pressed
                    sprintf(instruction, "NSKEY V%X", X);
                    break;
                    
                default:
                    printf ("Unknown opcode: 0x%X\n", opcode);
            }
            break;
    
        case 0xF000: 
            switch(opcode & 0x00FF) {
                case 0x0007: // Sets VX to the value of the delay timer
                    sprintf(instruction, "LOAD V%X, DELAY", X);
                    break;

                case 0x0015: // Sets the delay timer to VX
                    sprintf(instruction, "LOAD DELAY, V%X", X);
                    break;

                case 0x0018: // Sets the sound timer to VX
                    sprintf(instruction, "LOAD SOUND, V%X", X);
                    break;

                case 0x0029: // Sets I to the location of the sprite for the character in VX
                    sprintf(instruction, "LOAD DIGIT_SPRITE, V%X", X);
                    break;

                case 0x0033: // BCD, e.g. 245 -> *(I+0) = 2, *(I+1) = 4, *(I+2) = 5
                    sprintf(instruction, "STORE BCD, V%X", X);                    
                    break;
                    
                case 0x001E:  // Adds VX to I
                    sprintf(instruction, "ADD, I, V%X", X);                    
                    break;
                case 0x0055: // Stores V0 to VX (including VX) in memory starting at address I
                    sprintf(instruction, "REG_STORE, V%X", X);                                        
                    break;
                    
                case 0x0065: // Fills V0 to VX (including VX) with values from memory starting at address I
                    sprintf(instruction, "REG_LOAD, V%X", X);
                    break;

                default:
                    printf ("Unknown opcode: 0x%X\n", opcode);
            }
            break;
     
        default:
            printf ("Unknown opcode: 0x%X\n", opcode);
 
    }

    return instruction;
}
