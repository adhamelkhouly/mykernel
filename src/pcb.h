#ifndef PCB_H_
#define PCB_H_

typedef struct PCB {
     FILE* PC;
     int pageTable[10];
     int PC_page;
     int PC_offset;
     int pages_max;
     int backingstore_id;
     } PCB;

struct PCB* makePCB(FILE* p);

#endif
