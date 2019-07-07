#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "memorymanager.h"

typedef struct PARTITION {
 int total_blocks;
 int block_size;
} PARTITION;
PARTITION* myPartition;

typedef struct FAT {
 char *filename;
 int file_length;
 int blockPtrs[10];
 int current_location;
} FAT[20];
FAT* myFAT;

typedef struct ID_MAP {
    FILE* fp;
    int FAT_index;
} ID_MAP[5];
ID_MAP* myMap;

char* block_buffer;
FILE* fp[5];
char* current_partition = NULL;

// initialize all global data structure and variables
void initIO() {
    
    for (int i=0; i<5; i++) {
        fp[i] = NULL;
    }

    myMap = malloc(5*sizeof(ID_MAP));
    for (int i=0; i<5; i++) {
        myMap[i]->FAT_index = -1;
        myMap[i]->fp = NULL;
    }
    
    myPartition = malloc(sizeof(PARTITION));
    myPartition->block_size = 0;
    myPartition->total_blocks = 0;

    myFAT = malloc(20*sizeof(FAT));
    for (int i=0; i<20; i++) {
        myFAT[i]->filename = "";
        myFAT[i]->file_length = 0;
        memset(myFAT[i]->blockPtrs, -1, sizeof(myFAT[i]->blockPtrs));
        myFAT[i]->current_location = -1;
    }
}

// create & format partition, return success (1) or failure (0)
int partition(char *name, int blocksize, int totalblocks){
    char command[50];
    char file_path[200];
    char ch;
    char new_line[200];

    myPartition->block_size = blocksize;
    myPartition->total_blocks = totalblocks;

    snprintf(file_path, sizeof(file_path), "PARTITION/%s", name);
    system("mkdir -p PARTITION");
    memset(command, '\0', 50);

    // Check if file already exists
    FILE* check_file = fopen(file_path, "r");
    if (check_file != NULL) {
        printf("Partition already exists. Loading Partition...\n");
        return 0;
    }
    
    // Create the the new file
    FILE* file = fopen(file_path, "w");

    // Add Partition Info
    snprintf(new_line, sizeof(new_line), "%d %d\n", myPartition->block_size, myPartition->total_blocks);
    fputs(new_line, file);
    // memset(new_line, '\0', sizeof(new_line));

    // Add FAT Info (Empty)
    for (int i=0; i<20; i++) {
        fputs("\n", file);
    }

    for (int i=0; i<(blocksize*totalblocks); i++){
        fputs("0", file);
    }
    fclose(file);
    return 0;
}

// load FAT & create buffer_block, return success / failure
int mount(char* name) {
    initIO();
    char file_path[200];
    char line[1024];

    // Create the the new file
    snprintf(file_path, sizeof(file_path), "PARTITION/%s", name);
    FILE* file = fopen(file_path, "r+");

    if (file == NULL) {
        return 0;
    }

    current_partition = strdup(file_path);

    // Load partition information
    fgets(line, sizeof(line), file);
    myPartition->block_size = atoi(strtok(line, " "));
    myPartition->total_blocks = atoi(strtok(NULL, " "));

    // Load FAT info
    for (int i=0; i<20; i++) {
        fgets(line, sizeof(line), file);
        if (strcmp(line, "\n") != 0) { // Line is not empty
            char* copied_line = strdup(line);
            myFAT[i]->filename = strtok(copied_line, " ");

            myFAT[i]->file_length = atoi(strtok(NULL, " "));
            for (int j=0; j<10; j++) {
                myFAT[i]->blockPtrs[j] = atoi(strtok(NULL, " "));
            }
            // myFAT[i]->current_location = atoi(strtok(NULL, " "));
        }
    }

    block_buffer = malloc(sizeof(myPartition->block_size));
    return 0;
}

// find filename or creates file if it does not exist, returns file’s FAT index
int openfile(char *name) {
    int FAT_index = -1;
    int fp_index = -1;

    for (int i=0; i<20; i++) {
        if (strcmp(myFAT[i]->filename, name) == 0) {
            FAT_index = i;
            break;
        }
    }

    // File not in FAT, needs to be added
    if (FAT_index == -1) { 
        // Check that FAT is not full
        for (int i=0; i<20; i++) {
            if (strcmp(myFAT[i]->filename, "") == 0) {
                FAT_index = i;
                break;
            }
        }
        // FAT is Full
        if (FAT_index == -1) {
            return -1;
        }
        myFAT[FAT_index]->filename = name;
        return FAT_index;
    }
    for (int i=0; i<5; i++) {
        // fp is not full
        if (fp[i] == NULL){ 
            fp_index = i;
            break;
        }
    }
    // fp is full
    if (fp_index == -1) { 
        return -1;
    }
    // Open the partition file
    fp[fp_index] = fopen(current_partition, "r+");

    // Struct to remember indices
    int map_index = -1;
    for (int i=0; i<5; i++) {
        if (myMap[i]->FAT_index == -1) {
            map_index = i;
            break;
        }
    }
    
    myMap[map_index]->fp = fp[fp_index];
    myMap[map_index]->FAT_index = FAT_index;


    if (myFAT[FAT_index]->current_location == -1) {
        myFAT[FAT_index]->current_location = 0;
    }
    return FAT_index;
}

// using the file FAT index number, load buffer with data from blockID
int readBlock(int file) {
    if (current_partition == NULL) {
        printf("Mount a partition before writing!\n");
        return -1;
    } 
    if (myFAT[file]->file_length == 0) {
        return -1;
    }
    FILE* p;
    for (int i=0; i<5; i++) {
        if (myMap[i]->FAT_index==file) {
            p = myMap[i]->fp;
            break;
        }
    }

    if (p==NULL) {
        p = fopen(current_partition, "r+");
    }
    else {
        fseek(p, 0, SEEK_SET);
    }

    int start_index = myFAT[file]->blockPtrs[myFAT[file]->current_location];

     // Move pointer to last line
    char line[1024];
    for (int i=0; i<21; i++) {
        fgets(line, sizeof(line), p);
    }
    fseek(p,ftell(p),SEEK_SET);

    // Move pointer to first block character in last line
    for (int i=0; i<start_index; i++) {
        fgetc(p);
    }

    if (start_index == -1) {
        memset(block_buffer, '\0',sizeof(block_buffer));
        block_buffer = strdup("EOF");
    }
    else {
        char block[myPartition->block_size];
        memset(block, '\0',sizeof(block));
        for (int i=0; i<myPartition->block_size; i++) {
            if (!feof(p)) {
                block[i] = fgetc(p);
            }
            else {
                return -1;
            }
        }
        block_buffer = strdup(block);
        myFAT[file]->current_location++;
    }
    return 0;
}

// return block data as string from buffer_block
char *returnBlock() {
    return block_buffer;
}

// sensitive to block size, write data to disk at blockID
int writeBlock(int file, char *data) {
    if (current_partition == NULL) {
        printf("Mount a partition before writing!\n");
        return -1;
    } 

    FILE* p = NULL;
    for (int i=0; i<5; i++) {
        if (myMap[i]->FAT_index==file) {
            p = myMap[i]->fp;
            break;
        }
    }

    if (p==NULL) {
        p = fopen(current_partition, "r+");
    }
    else {
        fseek(p, 0, SEEK_SET);
    }

    int start_index = 0;
    if (file > 0) {
        for (int i=0; i<10; i++) {
            if ((myFAT[file-1]->blockPtrs[i]) != -1) {
                start_index = (myFAT[file-1]->blockPtrs[i]) + myPartition->block_size;
            }
        }
    }

    start_index = myFAT[file]->blockPtrs[myFAT[file]->current_location] + start_index;

     // Move pointer to last line
    char line[1024];
    for (int i=0; i<21; i++) {
        fgets(line, sizeof(line), p);
    }

    // Seek the first block
    fseek(p,ftell(p) + (start_index),SEEK_SET);
    

    // Update FAT Structure
    float file_length = strlen(data);
    int number_of_blocks = myceil(file_length/myPartition->block_size);
    
    for (int i=0; i<number_of_blocks; i++) {
        myFAT[file]->blockPtrs[i] = start_index + i*myPartition->block_size;
    }
    myFAT[file]->file_length = file_length;
    myFAT[file]->current_location += number_of_blocks;

    // If data is too long to fit in any empty space
    if ((myFAT[file]->file_length + myFAT[file]->blockPtrs[0]) > myPartition->block_size*myPartition->total_blocks) {
        printf("Memory full! You may be able to write a shorter string!\n");
        return -1;
    }
    // Write to data partition
    fputs(data, p);

    // Update FAT in partition file
    fseek(p, 0, SEEK_SET);
    memset(line, '\0', sizeof(line));
    for (int i=0; i<(file + 1); i++) {
        fgets(line, sizeof(line), p);
    }

    int last_position = ftell(p);
    int remove_length = 0;
    fgets(line, sizeof(line), p);

    if (strcmp(line, "\n") != 0) {
        return 0;
    }
    
    char old_file[1500];
    memset(old_file, '\0', sizeof(old_file));

    char updated_fat[1024];

    memset(updated_fat, '\0', sizeof(updated_fat));

    snprintf(updated_fat, sizeof(updated_fat), "%s %d %d %d %d %d %d %d %d %d %d %d %d", 
    myFAT[file]->filename, myFAT[file]->file_length,
    myFAT[file]->blockPtrs[0],
    myFAT[file]->blockPtrs[1],
    myFAT[file]->blockPtrs[2],
    myFAT[file]->blockPtrs[3],
    myFAT[file]->blockPtrs[4],
    myFAT[file]->blockPtrs[5],
    myFAT[file]->blockPtrs[6],
    myFAT[file]->blockPtrs[7],
    myFAT[file]->blockPtrs[8],
    myFAT[file]->blockPtrs[9],
    myFAT[file]->current_location);
    

    fseek(p, 0, SEEK_SET);
    char c = fgetc(p);
    int i = 0;
    while (c != EOF) {
        if(i == last_position){
            for(int j = 0; j < strlen(updated_fat); j++){
                old_file[i] = updated_fat[j];
                i++;
            }
        }
        if (i > last_position && i < (last_position + remove_length)) {
            old_file[i] = '\0';
        }
        else {
            old_file[i] = c;
        }
        c = fgetc(p);
        i++;
    }
    old_file[i] = '\0';

    // Dump the whole buffer
    fseek(p, 0, SEEK_SET);
    fputs(old_file, p);

    fflush(p);

    return 0;
}
