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

int execute(arglist *argstruct)
{
	int execstatus;
	int i;
	pid_t p;
	int *pipes;
	int count = 0;
	int pipenum;
	pipenum = countargs(argstruct);
	pipes = (int *) malloc(sizeof(int) * 2 * pipenum);


	/* Creates all the pipes needed to execute the full command */
	for (i = 0; i < pipenum * 2; i++)
	{
		if (pipe(pipes + i * 2) == -1)
		{
			fprintf(stderr, "Pipe %d failed.\n", i * 2);
			return -1;
		}
	}
	/* Each command in argstruct is executed */
	while (argstruct)
	{
		/* Forks, runs if fork() fails */
		if ((p = fork()) == -1)
		{
			fprintf(stderr, "fork failed.\n");
			return -1;
		}
		/* The child runs this */
		if (p == 0)
		{
			/* Sets the stdin for the current command to the previous
 			   command's stdout (if applicable) */
			if (count)
			{
				if (dup2(pipes[(count - 1) * 2], 0) == -1)
				{
					fprintf(stderr, "Error when using dup2 on input pipe %d.\n", (count - 1) * 2);
					return -1;	
				}
			}
			/* Sets the stdout for the current command to the next
 			   command's stdin (if applicable) */
			if (argstruct -> next)
			{
				if (dup2(pipes[count * 2 + 1], 1) == -1)
				{
					fprintf(stderr, "Error when using dup2 on output pipe %d.\n", count * 2 + 1);
					return -1;
				}
			}
			/* Close the pipes (stdin and stdout have been set) */
			for (i = 0; i < pipenum * 2; i++)
				close(pipes[i]);
			for (i = 0; i < argstruct->argc; i++)
				fprintf(stderr, "%s", argstruct->argv[i]);

			if (execvp(argstruct->argv[0], argstruct->argv) == -1)
			{
				fprintf(stderr, "execvp failed.\n");
				return -1;
			}
		}
		wait(&execstatus);
		argstruct = argstruct->next;
		count++;
	}
	
	/* Only the parent will make it to this point- close the pipes */
	for (i = 0; i < pipenum * 2; i++)
		close(pipes[i]);

	return 0;	
}
