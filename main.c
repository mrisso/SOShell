#include "csh.h"

int main (void)
{

	char **pName = malloc(sizeof(char*) * 6);
	int sig;
	pid_t pid;

	while(1)
	{
		printf("csh> ");

		sig = getCom(pName);

		switch(sig)
		{
		case WAIT_CMD:
			waitCmd();
			break;

		case EXIT_CMD:
			free(pName);
			printf("vou quitar\n");
			exitCmd();
			break;

		case EXEC_CMD:
			pid = execCmd(pName);
			waitpid(pid, NULL, WUNTRACED);
			break;

		case EXEC_ERR_ARGS:
			printf("Número máximo de argumentos (5) foi excedido\n");
			break;

		case EXEC_NULL:
			break;

		default:
			printf("Algo de errado não está certo\n");
			break;
		}
	}

	return 0;
}
