#include "csh.h"

int PID = 0; // PID do filho para tratamento de Ctrl-C
int foreground = FG_FALSE; // Booleano para tratar o Ctrl-C

// Funções da CSH

int getCom (char **pName) //pName precisa ser alocado antes como um vetor que contem 6 strings (de 0 a 5)
{
	char *comando = malloc(sizeof(char) * PROG_TAM);
	char *token = NULL;
	int i;
	char delim[2];
	delim[0] = ' ';
	delim[1] = '\0'; //Criar delimitador para strtok
	
    fgets(comando, PROG_TAM ,stdin);

	//Se a primeira posiçao for um \n ou um espaço, a entrada é considerada nula
	if(comando[0] == '\n' || comando[0] == ' ')
	{
		pName[0] = NULL;
		free(comando);
		return EXEC_NULL;
	}

	//Tirar o \n da string
	i = strlen(comando);
	comando[i-1] = '\0';

	if(DEBUG)
	{
		printf("%s\n",comando);
	}

	if(strcmp(comando,"wait") == 0) 
	{
		pName[0] = NULL;
		free(comando);
		return WAIT_CMD; //Sinal para executar wait
	}
	
	if(strcmp(comando,"exit") == 0) 
	{
		pName[0] = NULL;
		free(comando);
	    return EXIT_CMD; //Sinal para executar exit
	}

	//Separando a string em programa e argumentos
	token = strtok(comando, delim);
	if(token)
		pName[0] = strdup(token); //Duplicar string

	else
		return EXEC_NULL; //Se token for NULL, entrada vazia

	for(i = 1; i <= 5; i++)
	{
		if((token = strtok(NULL, delim)))
			pName[i] = strdup(token); //Duplicar string

		else
			pName[i] = NULL;
	}
			
	//Se sim, o programa tem mais de 5 argumentos
	if(strtok(NULL, delim) != NULL)
	{
		free(comando);
		return EXEC_ERR_ARGS; //Erro de numero de argumentos
	}

	free(comando);

	if(DEBUG)
	{
		for(i = 0; i <= 5; i++)
		{
			if(pName[i] != NULL)
				printf("%s\n", pName[i]);
		}
	}
	
	return EXEC_CMD; //Sinal para executar o programa requisitado
}

void waitCmd (void)
{
	int status;
	int count = 0;
	pid_t pid;

	while((pid = waitpid(-1, &status, WNOHANG)) > 0) //waitpid para todos os processos
	{
		count++;
		if(WIFEXITED(status) != 0) //Teste de morte por exit
		{
			printf("O processo %d morreu por um EXIT\n", pid);
		}

		else
		{
			printf("O processo %d morreu por um SIGNAL\n", pid);
		}
	}

	printf("Não existem mais processos no modo Zombie\n");
}

void exitCmd (void)
{
	//Wait para aguardar que os processos zumbis ou suspensos morram
	while(waitpid(-1, NULL, WNOHANG)>=0);

	//Exit para finalizar a shell
	exit(0);
}

pid_t execCmd (char **vCmd)
{
	pid_t pid;

	//Execução do fork para o processo em foreground (filho)
	if((pid = fork()) == -1)
	{
		printf("Não foi possível criar o processo filho (foreground)\n");
	}

	if(pid != 0) //Caso seja a csh, retorne o pid do filho
	{
		return pid;
	}

	else //Caso seja o filho da csh, faça um fork e execute
	{
		if((pid = fork()) == -1)
		{
			printf("Não foi possível criar o processo neto (background)\n");
		}

		if(pid == 0)
		{
			if(setsid() == -1) //Coloca o processo neto em uma nova sessao (background)
			{
				printf("Nao foi possivel colocar o neto em background\n");
			}

			else
			{
				if(execvp(vCmd[0], vCmd) == -1)//Executa o programa no neto e mata os processos em caso de erro
				{
					kill(getppid(), SIGTERM);
					printf("Comando não encontrado!\n");
					exit(1);			
				}
			}
		}

		else
		{
			manageSignals(UNBLOCK); //Todos os sinais ficam desbloqueados menos SIGINT
			if(execvp(vCmd[0], vCmd) == -1)//Executa o programa no filho e mata os processos em caso de erro
			{
				kill(pid, SIGTERM);
				printf("Comando não encontrado!\n");
				exit(1);			
			}
		}
	}	
	  
	return pid;
}

int manageSignals (int action)
{
	sigset_t mask;

	if(action == BLOCK) //Bloquear todos os sinais
	{
		sigfillset(&mask);
		return sigprocmask(SIG_SETMASK, &mask, NULL);
	}

	if(action == UNBLOCK) //Desbloquear todos os sinais menos SIGINT
	{
		sigemptyset(&mask);
		sigaddset(&mask, SIGINT);
		return sigprocmask(SIG_SETMASK, &mask, NULL);
	}

	return -1; //Acao nao reconhecida
}

void setForeground (int value)
{
	foreground = value; // Sinalizar se existe um processo em foreground
}

void setPid (pid_t pid)
{
	PID = pid; // Sinalizar PID do processo em foreground
}

void sigIntHandler (int dummy)
{
	char c;

	manageSignals(BLOCK); // Bloquear todos os sinais no tratamento
	
	if(foreground)
	{
		while(1)
		{
			printf("Não adianta me enviar um sinal por Ctrl-c, não vou morrer! Você quer suspender meu filho que está rodando em foreground? S/n: ");
			c = getc(stdin);

			if(c != '\n')
				getc(stdin);

			if(c == '\n' || c == 'S' || c == 's')
			{
				kill(PID,SIGSTOP); // Suspender processo em fg
				break;
			}

			else if(c == 'n' || c == 'N')
				break;
		}
	}

	manageSignals(UNBLOCK); // Desbloquear os sinais após o tratamento
}
