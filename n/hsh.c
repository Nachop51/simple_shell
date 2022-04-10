#include "main.h"

/**
 * main - Recreation of a "sh"
 *
 * Return: 0 If succeed, or the number of the error
 */
int main(void)
{
	size_t i = 0;
	int counter = 0, builtIn = 0, status = 0, exitValue = 0;
	char *buffer = NULL, **argv = NULL, *dup = NULL;
	pid_t child_pid;

	while (1)
	{
		_isattyAndSignal();
		counter = getline(&buffer, &i, stdin);
		if (counter == -1)
			free_and_exit(buffer);
		if (_checkChars(buffer) == -1)
			continue;
		buffer = clearBuffer(buffer, counter);
		builtIn = _checkBuiltIn(buffer);
		if (builtIn == 1)
		{
			exitValue = getReturnValue(buffer);
			if (exitValue >= 0)
				break;
			continue;
		}
		dup = _strdup(buffer);
		argv = tokenize(dup, builtIn);
		if ((builtIn == 0 && itsExecutable(argv[0]) == 0))
			child_pid = child_fork(child_pid, argv[0]);
		else
			child_pid = 1;
		if (child_pid == 0 && execve(argv[0], argv, environ) == -1)
		{
			perror(argv[0]);
			break;
		}
		if (child_pid != 0)
			waitAndFree(status, argv, dup);
	}
	if (builtIn != 1)
		free_array_dup(argv, dup);
	free(buffer);
	return (exitValue);
}

/**
 * itsExectuable - Checks if a given path is an executable
 * @path: Route of the file
 *
 * Return: If it is an executable file or not
 */
int itsExecutable(char *path)
{
	if (isDir(path) == 0)
	{
		perror(NULL);
		return (1);
	}
	if (access(path, X_OK))
		return (2);
	else
		return (0);
}

/**
 * isDir - Checks if the path is a directory
 * @path: Route of the file
 *
 * Return: If it is a directory or not
 */
int isDir(const char *path)
{
	struct stat st = {0};

	stat(path, &st);
	return (S_ISREG(st.st_mode));
}

/**
 * getReturnValue - Get exit value
 * @str: String to traverse
 *
 * Return: The exit value
 */
int getReturnValue(char *str)
{
	char *cpy = _strdup(str), *token;
	int exitValue = 0;

	token = _strtok(cpy, ' ');
	token = _strtok(NULL, ' ');
	if (token == NULL)
	{
		free(cpy);
		return (0);
	}
	if (_isdigit(token) == 0)
		exitValue = _atoi(token);
	else
	{
		free(cpy);
		write(STDERR_FILENO, "exit: Illegal number\n", 22);
		return (-1);
	}
	free(cpy);
	if (exitValue < 0)
	{
		write(STDERR_FILENO, "exit: Illegal number\n", 22);
		exitValue = (-1);
	}
	return (exitValue);
}

/**
 * _isdigit - Chekcs if there are only digits in a given string
 * @str: String to traverse
 *
 * Return: If it's only digits or not
 */
int _isdigit(char *str)
{
	int i = 0, flag = 0;

	while (str[i])
	{
		if (str[i] >= 48 && str[i] <= 57)
		{
			i++;
			continue;
		}
		else
		{
			flag++;
			break;
		}
	}
	return (flag);
}

/**
 * waitAndFree - Waits and frees things
 * @status: Status of the process
 * @argv: Array to free
 * @dup: Duplicated array to free
 *
 * Return: Returns the status of isatty
 */
void waitAndFree(int status, char **argv, char *dup)
{
	wait(&status);
	free_array_dup(argv, dup);
}

/**
 * child_fork - Forks a process
 * @child_pid: Process ID of the child process
 * @name: Name of the program
 *
 * Return: The child process ID
 */
int child_fork(pid_t child_pid, char *name)
{
	child_pid = fork();
	if (child_pid == -1)
	{
		perror(name);
		exit(EXIT_FAILURE);
	}
	return (child_pid);
}

/**
 * _isattyAndSignal - Looks for signals and checks the isatty function
 */
void _isattyAndSignal(void)
{
	signal(SIGINT, sig_handler);
	if ((isatty(STDIN_FILENO) == 1))
		write(STDOUT_FILENO, "$ ", 3);
}

/**
 * _checkChars - Looks for a character in the buffer
 * @str: The buffer
 * Return: If there's a char or not
 */
int _checkChars(char *str)
{
	int i = 0, r = -1;

	while (str[i])
	{
		if (str[i] != 32 && str[i] != 10 && str[i] != '\t')
		{
			r = 0;
			if (str[0] == ' ' && str[1] != ' ')
			{
				str = _strtok(str, ' ');
			}
			break;
		}
		i++;
	}
	return (r);
}

/**
 * clearBuffer - Removes the '\n' char, and looks for tabulations
 * @str: The buffer
 * @counter: Length of the string
 * Return: The clean buffer
 */
char *clearBuffer(char *str, int counter)
{
	str[counter - 1] = '\0';
	str = searchAndDestroy(str);
	/*str = searchAndReplace(str);*/
	return (str);
}

/*
 * searchAndReplace - Looks for a ~ and replaces it for the variable $HOME
 * ~/../home/shell/simple_shell/n/a.out
 * @str: String to traverse
 * Return: The modified string or just the string
char *searchAndReplace(char *str)
{
	int i = 0, tilde = 0, flag = 0;
	char *cpy = _strdup(str), *new = NULL, *rest = NULL, *token = NULL;
	char *concatenated = NULL;

	while (str[i])
	{
		if (str[i] == '~')
			tilde++;
		i++;
	}
	printf("Tilde:%d\n", tilde);
	if (tilde > 0)
	{
		free(str);
		while (tilde > 0)
		{
			i = 0;
			while (cpy[i])
			{
				printf("cpy[%d]:%c\n", i, cpy[i]);
				if (cpy[i] == '~')
				{
					if (flag == 0)
					{
						token = strtok(cpy, "~");
						flag++;
					}
					else
					{
						rest = strtok(NULL, "~");
						new = addTilde(token);
					}
				}
				i++;
			}
			tilde--;
			printf("Tilde:%d\n", tilde);
		}
	}
	printf("cpy:%s.\n", cpy);
	str = _strdup(cpy);
	free(cpy);
	return (str);
}*/

/*char *addTilde(char *new)
{
	char *home = NULL, *concatenated = NULL;

	printf("new:%s.\n", new);
	home = _getenv("HOME");
	concatenated = str_concat(new, home);
	free(home);
	printf("con:%s.\n", concatenated);
	return (concatenated);
}*/

/**
 * checkDir - Built-In checker for cd (cd function)
 * @str: String to compare
 *
 * Return: If there's a coincidence or not
 */
int checkDir(char *str)
{
	char *cpy = _strdup(str), *dir = NULL;
	int builtIn = 0, flag = 0;

	if (strcmp(_strtok(cpy, ' '), "cd") == 0)
	{
		dir = _strtok(NULL, ' ');
		if (dir == NULL)
		{
			dir = _getenv("HOME");
			flag++;
		}
		if (chdir(dir) != 0)
			perror(dir);
		builtIn++;
	}
	if (flag == 1)
		free(dir);
	free(cpy);
	return (builtIn);
}

/**
 * searchAndDestroy - Looks for a tabulation and erases it
 * @str: String to traverse
 *
 * Return: The modified string or just the string
 */
char *searchAndDestroy(char *str)
{
	int i = 0, tab = 0;

	while (str[i])
	{
		if (str[i] == '\t')
		{
			tab++;
		}
		i++;
	}
	i = 0;
	if (tab > 0)
	{
		while (str[i])
		{
			if (str[i] == '\t')
			{
				str[i] = ' ';
			}
			i++;
		}
	}
	return (str);
}

/**
 * _checkBuiltIn - Checks if there's a Built-In in the buffer or not
 * @str: String to traverse
 *
 * Return: If there's a coincidence or not
 */
int _checkBuiltIn(char *str)
{
	if (checkDir(str) == 1)
		return (3);
	if (checkExit(str) == 1)
		return (1);
	if (checkEnv(str) == 1)
		return (2);
	return (0);
}

/**
 * checkExit - Built-In checker for exit
 * @str: String to compare
 *
 * Return: If there's a coincidence or not
 */
int checkExit(char *str)
{
	char *cpy = _strdup(str);

	if (strcmp(_strtok(cpy, ' '), "exit") == 0)
	{
		free(cpy);
		return (1);
	}
	free(cpy);
	return (0);
}

/**
 * checkEnv - Built-In checker for env
 * @str: String to compare
 *
 * Return: If there's a coincidence or not
 */
int checkEnv(char *str)
{
	char *cpy = _strdup(str);

	if (strcmp(_strtok(cpy, ' '), "env") == 0)
	{
		free(cpy);
		printenv();
		return (1);
	}
	free(cpy);
	return (0);
}

/**
 * sig_handler - Signal handler for ^C
 * @signo: Signal number
 */
void sig_handler(__attribute__((unused))int signo)
{
	dprintf(STDOUT_FILENO, "\n$ ");
}

/**
 * args - Counts all the possibles arguments of a function
 * @str: String to traverse
 *
 * Return: The possibles arguments
 */
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
	return (counter);
}

/**
 * free_and_exit - Frees a buffer and exits the function
 * @buffer: Buffer to free
 */
void free_and_exit(char *buffer)
{
	if (isatty(STDIN_FILENO) == 1)
		write(STDOUT_FILENO, "\n", 1);
	free(buffer);
	exit(0);
}

/**
 * free_array_dup - Frees an array and the duplicated string
 * @array: Array to free
 * @dup: Duplicated string to free
 */
void free_array_dup(char **array, char *dup)
{
	int i = 0;

	while (array[i] != NULL)
	{
		free(array[i]);
		i++;
	}
	free(array[i]);
	free(array);
	free(dup);
}

/**
 * printenv - Prints out all the environment variables
 */
void printenv(void)
{
	int i = 0;

	while (environ[i])
	{
		printf("%s\n", environ[i++]);
	}
}
