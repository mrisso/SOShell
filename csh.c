#include "csh.h"

// Funções da CSH

int *getCom (char **pName)
{
}

void waitCmd (void)
{
}

void exitCmd (void)
{
}

pid_t execCmd (char **vCmd, int tam)
{
	pid_t pid;

	//Execução do fork para o processo em foreground (filho)
	pid = fork();

	if(pid != 0) //Caso seja a csh, retorne o pid do filho
	{
		return pid;
	}

	else //Caso seja o filho da csh, faça um fork e execute
	{
		pid = fork();

		if(tam == 1)
			if(execv(vCmd[0], NULL) == -1)
				printf("Não foi possível executar o programa especificado (FILHO)\n");

			else
		 {
			if(execv(vCmd[0], &vCmd[1]) == -1)
				printf("Não foi possível executar o programa especificado (NETO)\n");
			}
	}	
	  
	return pid;
}

int manageSignals (int action)
{
	if(action == BLOCK)
	{
	}

	if(action == UNBLOCK)
	{
	}
}
