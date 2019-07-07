#include<stdio.h>
#include<string.h>
#include<stdlib.h>

FILE *ram[10];

void addToRAM(FILE *p) {
    int i;
    for (i=0; i<=9; i++) {
        if (ram[i] == NULL) {
            ram[i] = p;
            break;
        }
    }
}
