#include <stdio.h>
#include <string.h>
#include "die.h"
#include "disassemble.h"

int main (int argc, char *argv[]) {
    
    if (argc < 2) die ("USAGE: test <filename>");
    
    FILE *fp;
    unsigned char buffer[4096];
    size_t ret;

    fp = fopen(argv[1], "rb");
    if (fp == NULL) die ("Can't open input file");

    fseek(fp, 0, SEEK_END);
    unsigned short fsize = ftell(fp);
    rewind(fp);
    
    ret = fread(buffer, 1, fsize, fp);
    if (ret < fsize) die ("io read error");
    fclose(fp);

    unsigned short opcode; 
    char instruction [20];
    memset(instruction, '\0', sizeof(instruction));
    
    for (short i = 0; i < fsize; i += 2) {
	opcode = buffer[i] << 8 | buffer[i + 1];    
	printf("%s\n", disassembleOpcode(opcode, instruction));
    }

    return 0;
}
