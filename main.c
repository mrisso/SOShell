#include "csh.h"

int main (void)
{

	signal(SIGINT, sigIntHandler);

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
			exitCmd();
			break;

		case EXEC_CMD:
			pid = execCmd(pName);
			setPid(pid);
			setForeground(1);
			waitpid(pid, NULL, WUNTRACED);
			setForeground(0);
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
