#ifndef KERNEL_H_
#define KERNEL_H_
#include<stdio.h>
#include "pcb.h"

void scheduler();
void memoryCleanup();
void deleteAllNodes();
PCB* myinit(FILE* p);

typedef struct node{
    PCB* data;
    struct node* next;
} node;
node* head;
node* tail;

#endif
