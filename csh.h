#ifndef CSH_H
#define CSH_H

#define WAIT_CMD 1
#define EXIT_CMD 2
#define EXEC_CMD 3
#define UNBLOCK  4
#define BLOCK    5

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Funções da CSH

int *getCom (char **);

void waitCmd (void);

void exitCmd (void);

pid_t execCmd (char **, int);

int manageSignals (int);

#endif
