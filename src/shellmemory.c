#include<stdio.h>
#include<string.h>
#include<stdlib.h>

char *vars[100]={NULL};
char *vals[100]={NULL};


int print(char *words[]) {
    int i;
    if (words[1] == NULL || words[2] != NULL) { // Passing an extra argument to print
        return 5;
    }
    for(i=0;i<100;i++) {
        // The if statement is to handle calling print when
        // no variable was set using the set command
        if (vars[i] != NULL){
            // If variable is found break and save the index
            if (strcmp(vars[i], words[1]) == 0) {
                break;
            }
        }
    }

    // If the index is 100 then the variable does not exist
    if (i == 100) {
        return 2;
    }

    else {
        printf("%s", vals[i]);
        printf("\n");
        return 0;
    }
}

int set(char *words[]) {
    int j;

    // If the user sends an incomplete command
    // e.g set VAR, an error will be thrown
    if (words[2] == NULL || words[2][0] == '\0' || words[1][0] == '\0') {
        return 3;
    }

    // Passing a multi word argument to set
    if (words[3] != NULL && words[3][0] != '\0') { 
        return 4;
    }

    for(j=0;j<100;j++) {
        // If memory is empty add new variable here
        if (vars[j]==NULL) {
            break;
        }
        // If the variable already exists overwrite
        else if (strcmp(vars[j],words[1])==0) {
            free(vars[j]);
            free(vals[j]);
            break;
        }
    }

    vars[j] = words[1];
    vals[j] = words[2];
    
    return 0;
}


