#include "main.h"

int main(void)
{
	size_t i = 0;
	int counter = 0, builtIn = 0, status = 0;
	char *buffer = NULL, **argv = NULL, *dup = NULL;
	pid_t child_pid;

	signal(SIGINT, sig_handler);
	while (1)
	{
		if((isatty(STDIN_FILENO) == 1))
			printf("$ ");
		counter = getline(&buffer, &i, stdin);
		if (counter == -1)
			free_and_exit(buffer);
		if (_checkChars(buffer) == -1)
			continue;
		buffer[counter - 1] = '\0';
		builtIn = _checkBuiltIn(buffer);
		if (builtIn == 1)
			break;
		dup = _strdup(buffer);
		argv = tokenize(dup, builtIn);
		child_pid = fork();
		if (child_pid == -1)
		{
			perror("./hsh: 1");
			return (1);
		}
		if (child_pid == 0)
		{
			if (execve(argv[0], argv, NULL) == -1)
			{
				perror("./hsh: 1");
				break;
			}
		}
		else
		{
			wait(&status);
			if((isatty(STDIN_FILENO) == 0))
				break;
			free_array_dup(argv, dup);
		}
	}
	if (builtIn != 1)
		free_array_dup(argv, dup);
	free(buffer);
	return (0);
}

int _checkChars(char *str)
{
	int i = 0, r = -1;

	while (str[i])
	{
		if (str[i] != 32 && str[i] != 10)
		{
			r = 0;
			if(str[0] == ' ' && str[1] != ' ')
			{
				str = strtok(str, " ");
			}
			break;
		}
		i++;
	}
	return (r);
}

int _checkBuiltIn(char *str)
{
	if (strcmp(strtok(str, " "), "exit") == 0)
		return (1);
	if (strcmp(strtok(str, " "), "env") == 0)
	{
		printenv();
		return (2);
	}
	return (0);
}

void sig_handler(__attribute__((unused))int signo)
{
	dprintf(STDOUT_FILENO, "\n$ ");
}

int args(char *str)
{
	int i = 1, counter = 0;

	if (str[0] != ' ')
			counter++;
	if (str[0] == ' ' && (str[1] != ' ' && str[1] != '\0'))
		counter++;
	while (str[i])
	{
		if (str[i] == ' ' && (str[i + 1] != ' ' && str[i + 1] != '\0'))
			counter++;
		i++;
	}
	printf("%d\n", counter);
	return (counter);
}

void free_and_exit(char *buffer)
{
	dprintf(STDOUT_FILENO, "\n");
	free(buffer);
	exit(0);
}

void free_array_dup(char **array, char *dup)
{
	int i = 0;

	while(array[i] != NULL)
	{
		free(array[i]);
		i++;
	}
	free(array[i]);
	free(array);
	free(dup);
}

void printenv(void)
{
	int i = 0;

	while (environ[i])
	{
		printf("%s\n", environ[i++]);
	}
}