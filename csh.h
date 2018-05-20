#ifndef CSH_H
#define CSH_H

#define DEBUG         0 // Caso seja 1, habilitar prints de debug

// Tamanho para a string que captura o comando
#define PROG_TAM      256 

// Constantes para o retorno de getCom
#define WAIT_CMD      1
#define EXIT_CMD      2
#define EXEC_CMD      3
#define EXEC_ERR_ARGS 4
#define EXEC_NULL     5

// Constantes para o tratamento de sinais
#define UNBLOCK       6
#define BLOCK         7

// Constantes para o tratamento de foreground
#define FG_TRUE       1
#define FG_FALSE      0

// Includes
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

void setForeground (int);

void setPid (pid_t);

void sigIntHandler (int);

#endif
