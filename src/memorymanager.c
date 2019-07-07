#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include"ram.h"
#include"kernel.h"

int file_counter = 1;

int myceil(float input) {
    int val = (int)input;
    if (input == (float)val) {
        return val;
    }
    return val + 1;
}

int updatePageTable(PCB* p, int pageNumber, int frameNumber, int victimFrame) {
    if (frameNumber == -1) {
        node* cursor = head;
        // Find Victim Frame and update it
        while(cursor != NULL){
            for (int i=0; i<10; i++) {
                if (cursor->data->pageTable[i] == victimFrame) {
                    cursor->data->pageTable[i] = -99;
                    break;
                }
            }
            cursor = cursor->next;
        }

        p->pageTable[pageNumber] = victimFrame;
    }
    else {
        p->pageTable[pageNumber] = frameNumber;
    }
    return 0;
}

int updateFrame (int frameNumber, int victimFrame, FILE* page) {
    if (frameNumber == -1) {
        ram[victimFrame] = page;
    }
    else {
        ram[frameNumber] = page;
    }
    return 0;
}

int findVictim(PCB* p) {
    int r = rand() % 10;
    int in_pcb = 0;
    int first_run = 1;
    while (in_pcb || first_run) {
        first_run = 0;
        in_pcb = 0;
        for (int i=0; i<10; i++) {
            if (p->pageTable[i] == r) {
                in_pcb = 1;
            }
        }
        if (in_pcb == 1) {
            r = (r+1) % 10;
        }
    }
    return r;
}

int findFrame() {
    int full = 1;
    int i;
    for (i=0; i<10; i++) {
        if (ram[i] == NULL ) {
            full = 0;
            break;
        }
    }
    if (full) {
        return -1;
    }
    else {
        return i;
    }
}

FILE* findPage(int pageNumber, FILE* f) {
    char line[1000];
    FILE* fp2 = fdopen(dup(fileno(f)), "r");
    rewind(fp2);
    for (int i=1; i <= 4*(pageNumber); i++){
        fgets(line, sizeof(line), fp2);
    }
    rewind(f);
    return fp2;
}


int countTotalPages(FILE *f) {
    int pages;
    char ch;
    double lines = 0;


    while(!feof(f)){
        ch = fgetc(f);
        if(ch == '\n') {
            lines++;
        }
    }
    lines++;
    pages = myceil(lines / 4.0);
    return pages;
}

int launcher(FILE* p) {
    char command[200];
    char file[200];
    char ch;
    char* target_path;

    // Create the the new file
    snprintf(file, sizeof(file), "BackingStore/p%d.txt", file_counter);
    snprintf(command, sizeof(command), "touch %s", file);
    system(command);

    // Save target path for reuse
    target_path = strdup(file);
    FILE* target_file = fopen(target_path, "w");

    // Copy file to a new one and rewind
    while(( ch = fgetc(p)) != EOF )
        fputc(ch, target_file);
    fclose(target_file);
    fclose(p);

    // Open new file in backing store and use that
    p = fopen(target_path, "r");
    int total_pages = countTotalPages(p);
    fseek(p, 0, SEEK_SET);

    // If the file is too big to fit in RAM
    // i.e more that 40 lines
    if (total_pages > 10 ) {
        return 12;
    }

    // Create a PCB and add it to the ready queue
    myinit(p);
    PCB* createdPCB = tail->data;
    createdPCB->pages_max = total_pages;
    createdPCB->PC_offset = 0;
    createdPCB->PC_page = 0;
    createdPCB->backingstore_id = file_counter;

    // Load one or two pages into RAM
    int frameNumber = findFrame();
    int victimFrame = -1;
    FILE* page;
    for (int i=0; i<=(1 - total_pages%2); i++) {
        frameNumber = findFrame();
        // If RAM is FULL
        if (frameNumber == -1) {
            victimFrame = findVictim(createdPCB);
        }
        page = findPage(i, p);
        updateFrame(frameNumber, victimFrame, page);
        updatePageTable(createdPCB, i, frameNumber, victimFrame);
    }

    // Increment file pointer for upcoming files
    file_counter++;
    return 0;
}
