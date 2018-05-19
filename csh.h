#ifndef CSH_H
#define CSH_H

#define PROG_TAM      256

#define WAIT_CMD      1
#define EXIT_CMD      2
#define EXEC_CMD      3
#define EXEC_ERR_ARGS 4
#define EXEC_NULL     5

#define UNBLOCK       5
#define BLOCK         6

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Funções da CSH

int getCom (char **);

void waitCmd (void);

void exitCmd (void);

pid_t execCmd (char **);

int manageSignals (int);

#endif
