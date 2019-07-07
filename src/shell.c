#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <string.h>
#include "interpreter.h"

int parseInput(char ui[]) {
    char tmp[1000];
    int a,b;
    char *words[1000] = {NULL}; 
    int w=0;

    // Skip white spaces
    for(a=0; ui[a]==' ' && a<1000; a++); 

    // Separate words by spaces
    while(ui[a] != '\0' && a<1000) {
        for(b=0; ui[a]!='\0' && ui[a]!=' ' && a<1000; a++, b++)
            tmp[b] = ui[a];
        tmp[b] = '\0';
        words[w] = strdup(tmp);
        a++; w++;
    }
    
    return interpreter(words);
}

void errorHandler(int errorResult) {
        // Error Message Handling
        if (errorResult == 1) {
            printf("Unknown command\n");
        }
        else if (errorResult == 2) {
            printf("Variable does not exist\n");
        }
        else if (errorResult == 3) {
            printf("set command takes 2 argument\n");
        }
        else if (errorResult == 4) {
            printf("set command only takes 2 arguments\n");
        }
        else if (errorResult == 5) {
            printf("print command takes 1 argument\n");
        }
        else if (errorResult == 6) {
            printf("help command does not take any additional arguments\n");
        }
        else if (errorResult == 7) {
            printf("quit command does not take any additional arguments\n");
        }
        else if (errorResult == 8) {
            printf("run command takes an argument\n");
        }
        else if (errorResult == 9) {
            printf("Script not found\n");
        }
        else if (errorResult == 10) {
            printf("exec takes up to 3 arguments\n");
        }
        else if (errorResult == 11) {
            printf("exec command takes a maximum of 3 arguments\n");
        }
        else if (errorResult == 12) {
            printf("mount takes 4 arguments\n");
        }
        else if (errorResult == 13) {
            printf("read takes 2 arguments\n");
        }
        else if (errorResult == 14) {
            printf("write takes 2 arguments\n");
        }
        else if (errorResult == 15) {
            printf("block must be of format [bunch of words]\n");
        }
        else if (errorResult == 16) {
            printf("mount arguments block size and total blocks must be ints and bigger than 0!\n");
        }
        else if (errorResult == 17) {
            printf("All 5 OS pointers have been used. Re-mount the partition!\n");
        }
        else if (errorResult == 18) {
            printf("You cannot read a file before writing it!\n");
        }
        else if (errorResult == -1) {
            exit(0);
        }
}

int ui(int argc, char const *argv[]) {
    int i, errorResult=0;
    char userInput[1000];
    printf("Welcome to the Adham shell !\n");
    printf("Shell version 3.0 Updated April 2019\n");

    while(1)
    {   
        // Input using keyboard and terminal
        if (isatty(STDIN_FILENO))
            printf("$");
        
        // Check that fget is not null
        char* check = fgets(userInput, sizeof(userInput), stdin);
        if (check == NULL) {
            freopen("/dev/tty", "r", stdin);
            continue;
        }
        // Strip trailing tokens
        userInput[strcspn(userInput, "\n")] = '\0';
        userInput[strcspn(userInput, "\r")] = '\0';

        // Input was redirected
        if (isatty(STDIN_FILENO) == 0) {
            printf("$%s\n", userInput);
        }

        errorHandler(parseInput(userInput));


    }

    return 0;
}
