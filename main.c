#include "csh.h"

int main (void)
{
	signal(SIGINT, sigIntHandler);

	// Vetor de strings para receber o comando da função getCom
	char **pName = malloc(sizeof(char*) * 6); 
	int sig; // Retorno de getCom
	pid_t pid;

	while(1)
	{
		printf("csh> ");

		sig = getCom(pName);

		switch(sig) // Executar o comando tratado por getCom 
		{
		case WAIT_CMD:
			waitCmd();
			break;

		case EXIT_CMD:
			free(pName);
			exitCmd();
			break;

		case EXEC_CMD:
			pid = execCmd(pName);
			setPid(pid); //Sinalizar pid do processo em fg
			setForeground(1); //Sinalizar um processo em fg
			//Waitpid sensível a mudança de estado
			waitpid(pid, NULL, WUNTRACED);
			setForeground(0); //Sinalizar processo em fg inexistente
			break;

		case EXEC_ERR_ARGS: // Número de argumentos maior que 5
			printf("Número máximo de argumentos (5) foi excedido\n");
			break;

		case EXEC_NULL: //Nenhuma entrada
			break;

			//Esta mensagem nunca aparecerá.
		default:
			printf("Função getCom teve retorno inesperado\n");
			break;
		}
	}

	return 0;
}
