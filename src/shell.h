#ifndef SHELL_H_
#define SHELL_H_

int parseInput(char ui[]);
void errorHandler(int errorResult);
int ui(int argc, char const *argv[]);
#endif