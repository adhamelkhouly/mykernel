#ifndef LAUNCHER_H_
#define LAUNCHER_H_
#include"pcb.h"

int launcher(FILE *p);
int updatePageTable(PCB* p, int pageNumber, int frameNumber, int victimFrame);
int updateFrame (int frameNumber, int victimFrame, FILE* page);
int findVictim(PCB* p);
int findFrame();
FILE* findPage(int pageNumber, FILE* f);
int countTotalPages(FILE *f);
int myceil(float input);
#endif
