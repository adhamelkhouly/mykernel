#ifndef DISKDRIVER_H_
#define DISKDRIVER_H_

int partition(char *name, int blocksize, int totalblocks);
int mount(char* name);
int openfile(char *name);
int readBlock(int file);
int writeBlock(int file, char *data);
char *returnBlock();
void initIO();

#endif