#include "csh.h"

#define DEBUG 0

int PID = 0;
int foreground = 0;

// Funções da CSH

int getCom (char **pName) //pName precisa ser alocado antes como um vetor que contem 6 strings (de 0 a 5)
{
	char *comando = malloc(sizeof(char) * PROG_TAM);
	char *token = NULL;
	int i;
	char delim[2];
	delim[0] = ' ';
	delim[1] = '\0';
	
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
		pName[0] = strdup(token);

	else
		return EXEC_NULL; //Se token for NULL, entrada vazia

	for(i = 1; i <= 5; i++)
	{
		if((token = strtok(NULL, delim)))
			pName[i] = strdup(token);

		else
			pName[i] = NULL;
	}
			
	//Se sim, o programa tem mais de 5 argumentos
	if(strtok(NULL, delim) != NULL)//Se sim, o programa tem mais de 5 argumentos
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
	pid_t pid;

	while((pid = waitpid(-1, &status, WNOHANG)) > 0) //waitpid para todos os processos
	{
		if(WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS) //Teste de morte por sinal
		{
			printf("O processo %d morreu por um SINGAL\n", pid);
		}

		else
		{
			printf("O processo %d morreu por um EXIT\n", pid);
		}
	}
}

void exitCmd (void)
{
	//Wait para aguardar que os processos zumbis morram
	while(waitpid(-1, NULL, WNOHANG)>=0);

	//Exit para finalizar a shell
	exit(0);
}

pid_t execCmd (char **vCmd)
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

		if(pid == 0)
		{
			if(setsid() == -1) //Coloca o processo em uma nova sessao (background)
			{
				printf("Nao foi possivel colocar o neto em background\n");
			}

			else
			{
				if(execvp(vCmd[0], vCmd) == -1)//Executa o programa no neto e mata os processos em caso de erro
				{
					kill(getppid(), SIGTERM);
					printf("Programa não encontrado1!\n");
					exit(1);			
				}
			}
		}

		else
		{
			if(execvp(vCmd[0], vCmd) == -1)//Executa o programa no neto e mata os processos em caso de erro
			{
				kill(pid, SIGTERM);
				printf("Programa não encontrado2!\n");
				exit(1);			
			}

			waitpid(pid,NULL,0);
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

	if(action == UNBLOCK) //Desbloquear todos os sinais
	{
		sigemptyset(&mask);
		return sigprocmask(SIG_SETMASK, &mask, NULL);
	}

	return -1; //Acao nao reconhecida
}

void setForeground (int value)
{
	foreground = value;
}

void setPid (pid_t pid)
{
	PID = pid;
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

			if(c == '\n' || c == 'S' || c == 's')
			{
				//Suspender filho

				break;
			}

			else if(c == 'n' || c == 'N')
				break;
		}
	}

	manageSignals(UNBLOCK);
}
