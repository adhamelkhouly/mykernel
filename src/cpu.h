#ifndef CPU_H_
#define CPU_H_
#include<stdio.h>

typedef struct CPU {
    FILE *IP;
    char IR[1000];
    int quanta;
    int offset;
    } CPU;

int cpu_run(int quanta);

#endif
