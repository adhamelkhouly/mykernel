#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "shell.h"
#include "ram.h"
#include "pcb.h"
#include "cpu.h"
#include "memorymanager.h"
#include<time.h>
#include "DISK_driver.h"
extern CPU* myCPU;

typedef struct node{
    PCB* data;
    struct node* next;
} node;

node* head;
node* tail;

node* createNode(PCB* data, node* next)
{
    node* created_node = malloc(sizeof(node));
    created_node->data = data;
    created_node->next = next;

    return created_node;
}

// Deletes all linked list nodes
void deleteAllNodes()
{
    node *cursor, *tmp;

    if(head != NULL)
    {
        cursor = head->next;
        head->next = NULL;
        while(cursor != NULL)
        {
            tmp = cursor->next;
            free(cursor);
            cursor = tmp;
        }
    }
}

void addToReady(PCB* created_PCB) {
    node *cursor = head;
    // If it's the first PCB
    if (head == NULL) {
        head = createNode(created_PCB, NULL);
        tail = head;
    }
    else {
        // Find the last node
        while( cursor->next != NULL){
            cursor = cursor->next;
        }

        cursor->next =  createNode(created_PCB,NULL);
        tail = cursor->next;
    }
}

// Remove all mallocd files
void memoryCleanup() {
        node* old_head = head;
        head = head->next;
        char command[200];
        for (int i=0; i<10; i++) {
            if (old_head->data->pageTable[i] != -99) {
                int frameNumber = old_head->data->pageTable[i];
                fclose(ram[frameNumber]);
                ram[frameNumber] = NULL;
            }
        }
        snprintf(command, sizeof(command), "rm BackingStore/p%d.txt", old_head->data->backingstore_id);
        system(command);
        free(old_head->data);
        free(old_head);
}

void boot() {
    char command[50];
    strcpy( command, "rm -r BackingStore" );
    system(command);
    memset(command, '\0', 50);
    strcpy(command, "mkdir BackingStore");
    system(command);
    initIO();
}

void scheduler() {
    int program_complete;
    myCPU = malloc(sizeof(CPU));
    while(head != NULL) {
        
        myCPU->quanta = 2;
        myCPU->IP = head->data->PC;
        myCPU->offset = head->data->PC_offset;
        head->data->PC_offset += 2;
        program_complete = cpu_run(myCPU->quanta);
        // If program is completed
        // Proceed with memory cleanup
        if (program_complete == 1) {
            memoryCleanup(head);
        }
        // If program is not complete add it to the back
        else if (program_complete == 0) {
            addToReady(head->data);
            node* old_head = head;
            head = head->next;
            free(old_head);
        }
        else if (program_complete == 2) {
            int PC_page = ++head->data->PC_page;
            // If program is done, all pages were run
            if (PC_page > head->data->pages_max) {
                memoryCleanup(head);
                continue;
            }
            // Else if the next page exists in RAM
            else if (head->data->pageTable[PC_page] != -99) {
                int frame = head->data->pageTable[PC_page];
                head->data->PC = ram[frame];
                head->data->PC_offset = 0;
            }
            // Else the next page is not in RAM
            else {
                FILE* page = findPage(PC_page, head->data->PC);
                int frameNumber = findFrame();
                int victimFrame = -1;
                frameNumber = findFrame();
                // If RAM is FULL
                if (frameNumber == -1) {
                    victimFrame = findVictim(head->data);
                }
                updateFrame(frameNumber, victimFrame, page);
                updatePageTable(head->data, head->data->PC_page, frameNumber, victimFrame);
                int frame = head->data->pageTable[PC_page];
                head->data->PC = ram[frame];
                head->data->PC_offset = 0;
            }
            // Add node to the back
            addToReady(head->data);
            node* old_head = head;
            head = head->next;
            free(old_head);
        }
    }
    // Delete all linked list nodes
    deleteAllNodes();
    // Free the CPU
    free(myCPU);
}

void myinit(FILE *p) {
    PCB* createdPCB = makePCB(p);
    addToReady(createdPCB);
}

int main(int argc, char const *argv[]) {
    printf("Kernel 3.0 loaded!\n");
    srand(time(NULL));
    boot();
    ui(argc, argv);
    return 0;
}
