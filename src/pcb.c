#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "ram.h"

typedef struct PCB {
     FILE* PC;
     int pageTable[10];
     int PC_page;
     int PC_offset;
     int pages_max;
     int backingstore_id;
     } PCB;

struct PCB* makePCB(FILE* p) {
    PCB* created_pcb = malloc(sizeof(PCB));
    created_pcb->PC = p;
    for (int i=0; i<10; i++) {
        created_pcb->pageTable[i] = -99;
    }

    return created_pcb;
}
