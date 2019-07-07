#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "shell.h"
#include "kernel.h"

typedef struct CPU {
    FILE *IP;
    int quanta;
    char IR[1000];
    int offset;
    } CPU;
CPU* myCPU;


int cpu_run(int quanta) {
    int errCode = 0;
    int counter = 0;
    int null_fget = 0;
    while(!feof(myCPU->IP)) {
        if (counter >= quanta) {
            break;
        }
        char* ret = fgets(myCPU->IR,sizeof(myCPU->IR),myCPU->IP);
        if (ret == NULL){
            null_fget = 1;
            break;
        }
        myCPU->offset++;
        myCPU->IR[strcspn(myCPU->IR, "\n")] = '\0';  // Removing trailing \n due to fgets
        myCPU->IR[strcspn(myCPU->IR, "\r")] = '\0';
        errCode = parseInput(myCPU->IR);

        if (errCode != 0) {
            // Quit is called from a program
            if (errCode == -1) {
                memoryCleanup();
                // Delete all linked list nodes
                deleteAllNodes();
                // Free the CPU and malloc a new one
                free(myCPU);
            }
            errorHandler(errCode);
        }
        memset(myCPU->IR, '\0', 1000);
        counter = counter + 1;
        if (feof(myCPU->IP) || null_fget) {
            return 1;
        }
        if (myCPU->offset == 4) {
            return 2;
        }
    }
    if (feof(myCPU->IP) || null_fget) {
        return 1;
    }
    else {
        return 0;
    }
}
