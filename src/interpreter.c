#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include <unistd.h>
#include <assert.h>
#include "shell.h"
#include "shellmemory.h"
#include "kernel.h"
#include "memorymanager.h"
#include "DISK_driver.h"
#include "IOSCHEDULER.h"


// Prints command list and their descriptions
void help() {
    printf("COMMAND\t\t\t\t\t\t\tDESCRIPTION\n\n");
    printf("help\t\t\t\t\t\t\tDisplay all the commands\n");
    printf("quit\t\t\t\t\t\t\tExits / terminates the shell with “Bye!”\n");
    printf("set VAR STRING\t\t\t\t\t\tAssigns a value to shell memory\n");
    printf("print VAR\t\t\t\t\t\tPrints the STRING assigned to VAR\n");
    printf("run SCRIPT.TXT\t\t\t\t\t\tExecutes the file SCRIPT.TXT\n");
    printf("exec p1 p2 p3\t\t\t\t\t\tExecutes concurrent programs\n");
    printf("\t\t\t\t\t\t\t$ exec prog.txt prog2.txt\n");
    printf("mount partitionName number_of_blocks block_size\t\tmounts a partition\n");
    printf("write filename [a bunch of words]\t\t\twrites to a file\n");
    printf("read filename variable\t\t\t\t\treads a file block into variable\n");
    
}

// Terminates the shell
int quit() {
    printf("Bye!\n");
    return -1;
}

int run(char *words[]) {
    int errCode = 0;
    char line[1000];

    FILE *p = fopen(words[1], "r");

    if(p==NULL){ // If file does not exist
        return 9;
	}

    // fgets(line,sizeof(line),p);
    while(!feof(p)) {
        char* ret = fgets(line,sizeof(line),p);
        if (ret == NULL){
            break;
        }
        line[strcspn(line, "\n")] = '\0';  // Removing trailing \n due to fgets
        line[strcspn(line, "\r")] = '\0';
        errCode = parseInput(line);
        if (errCode != 0) {
            errorHandler(errCode);
        }
        memset(line, '\0', 1000);

    }
    fclose(p);
    return 0;
}

int exec(char *words[]) {
    int i;
    // Passing no arguments to exec
    if (words[1] == NULL) {
        return 10;
    }

    // Passing a 4 word argument to exec
    if (words[4] != NULL && words[4][0] != '\0') {
        return 11;
    }

    // Calling Kernel to load programs and create PCBs
    for (i=1; i<=3; i++) {
        if (words[i] != NULL) {
            FILE *p = fopen(words[i], "r");
            if (p==NULL){ // If file does not exist
                printf("Script not found\n");
	        }
            else {
                int errCode = launcher(p);
                if (errCode == 12) { 
                    printf("Program is too big to fit in RAM\n");
                }
            }
        }
    }
    // Calling Scheduler
    scheduler();
    
    return 0;
}

int prePartition(char *words[]) {
    if (words[3] == NULL || words[3][0] == '\0') {
        return 12;
    }

    // Passing more than 3 arguments to mount
    if (words[4] != NULL && words[4][0] != '\0') { 
        return 12;
    }
    if ( atoi(words[2]) == 0 ||  atoi(words[3]) == 0) {
        return 16;
    }
    partition(words[1], atoi(words[2]), atoi(words[3]));
    mount(words[1]);
    return 0;
}

int read_wrapper(char *words[]) {
    if (words[2] == NULL || words[2][0] == '\0' || words[1][0] == '\0') {
        return 13;
    }
    file_id = openfile(words[1]);
    char* return_value = IOscheduler(NULL, NULL, 0);

    words[0] = "set";
    words[1] = words[2];
    words[2] = return_value;
    if (strcmp(return_value, "All 5 pointers have been used. Re-mount the partition!") == 0 ){
        return  17;      
    }
    else if (strcmp(return_value, "-99") == 0) {
        return 18;
    }
    set(words);

    return 0;
}

void remove_start_end(char *str) {
    size_t len = strlen(str);
    assert(len >= 2);
    memmove(str, str+1, len-2);
    str[len-2] = 0;
}

int write_wrapper(char *words[]) {
    if (words[2] == NULL || words[2][0] == '\0' || words[1][0] == '\0') {
        return 14;
    }
    char* str = words[2];
    int i = 3;
    while (words[i] != NULL && words[i][0] != '\0') {
        str = strcat(str, " ");
        str = strcat(str, words[i]);
        i++;
    }
    if (words[2][0] != '[' || words[i-1][strlen(words[i-1])-1] != ']') {
        return 15;
    }
    remove_start_end(str);
    file_id = openfile(words[1]);
    IOscheduler(str, NULL, 1);
    return 0;
}


// Function similar to the one in lecture slides, but
// was modified for this assignment
int interpreter(char *words[]) {
    int errCode = 0;
    if (words[0] == NULL) // To catch users pressing enter
        return 0;

    else if (strcmp(words[0], "run") == 0){
        if (words[1] != NULL)
            errCode = run(words);
        else
            errCode = 8;
    }
    else if (strcmp(words[0], "exec") == 0){

        errCode = exec(words);
    }
    else if (strcmp(words[0], "help") == 0) {
        if (words[1] != NULL && words[1][0] != '\0')
            errCode = 6;
        else
            help();
    }
    else if (strcmp(words[0], "quit") == 0) {
        if (words[1] != NULL && words[1][0] != '\0') {
            return 7;
        }
        errCode = quit();
    }
    else if (strcmp(words[0], "set") == 0) {
        errCode = set(words);
    }
    else if (strcmp(words[0], "print") == 0) {
         errCode = print(words);
    }
    else if (strcmp(words[0], "mount") == 0) {
        errCode = prePartition(words);
    }
    else if (strcmp(words[0], "write") == 0) {
        errCode = write_wrapper(words);
    }
    else if (strcmp(words[0], "read") == 0) {
        errCode = read_wrapper(words);
    }
    else {
        errCode = 1;
    }

    return errCode;
}
