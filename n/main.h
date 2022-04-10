#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

/**
 * struct list_s - lista
 * @str: str
 * @next: next
 */

typedef struct list_s
{
	char *str;
	struct list_s *next;
} list_t;

/* String Functions */
int _strlen(const char *s);
char *_strdup(const char *str);
int _strcmp(const char *s1, const char *s2);
char *str_concat(char *s1, char *s2);
char *_strcpy(char *dest, char *src);
char *_strtok(char *str, char delim);

void _isattyAndSignal(void);
char *_getenv(const char *name);
void printenv(void);
int _checkChars(char *str);
char *path(char *filename);
int exists(char *path);
int is_regular_file(const char *path);
void sig_handler(int signo);
int _atoi(char *s);
int checkDir(char *str);
int args(char *str);
int _isdigit(char *str);
int checkExit(char *str);
char *searchAndReplace(char *str);
int checkRoute(char *str);
int getReturnValue(char *str);
char *clearBuffer(char *str, int counter);
int getLineAndCheck(char *str);
char *searchAndDestroy(char *str);
int itsExecutable(char *str);
int isDir(const char* fileName);
char *addTilde(char *new);
int child_fork(pid_t child_pid, char *name);
int checkEnv(char *str);
void waitAndFree(int status, char **argv, char *dup);
void free_array_dup(char **array, char *dup);
void free_and_exit(char *str);
int _checkBuiltIn(char *str);
char **tokenize(char *str, int builtIn);
#endif
