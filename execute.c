/* Jennifer Maunder and Richa Gadgil
 * CPE 357-03
 * Project 6
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "header.h"


int countargs(arglist *argstruct)
{
	int count = 0;
	while (argstruct != NULL)
	{
		count++;
		argstruct = argstruct->next;
	}
	return count;
}

int launchcmd(int origfd, arglist *argstruct)
{
	int fd[2];
	pid_t pid;
	pipe(fd);
	if ((pid = fork()) < 0)
	{
		fprintf(stderr, "Fork failed\n");
		return -1;
	}
	/* Child runs this */
	if (pid == 0)
	{
		/* Don't need to dup2 read if it's stdin */
		if (origfd != 0)
		{
			dup2(fd[0], 0);
		}
		/* Don't need to dup2 write if it's stdout */
		if (argstruct->next != NULL)
		{
			dup2(fd[1], 1);
		}
		if (execvp(argstruct->argv[0], argstruct->argv) < 0)
		{
			fprintf(stderr, "Exec failed\n");
			return -1;
		}
	}
	close(fd[0]);
	close(fd[1]);
	return fd[0];
}

int execute(arglist *argstruct)
{
	int argnum;
	int i;
	int origfd = 0;
	argnum = countargs(argstruct);
	for (i = 0; i < argnum; i++)
	{
		origfd = launchcmd(origfd, argstruct);
		argstruct = argstruct->next;
		if (origfd < 0)
			return -1;
	}
	return 0;
}
