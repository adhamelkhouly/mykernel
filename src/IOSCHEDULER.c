#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "pcb.h"
#include "shellmemory.h"
#include "DISK_driver.h"
int file_id;
typedef struct IO{
    char* data;
    PCB* ptr;
    int cmd;
} IO;
IO* queue;

char* IOscheduler(char *data, PCB *ptr, int cmd) {
    queue = malloc(sizeof(IO));
    queue->data = data;
    queue->ptr = ptr;
    queue->cmd = cmd;

    if (file_id != -1) {
        if (cmd==0) {
            if (readBlock(file_id) != -1) {
                return returnBlock();
            }
            else {
                return "-99";
            }
        }
        else {
            writeBlock(file_id, data);
        }
    }
    else {
        return "All 5 pointers have been used. Re-mount the partition!";
    }
    free(queue);
}