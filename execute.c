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
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


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

int launchcmd(int origfd, int newfd, arglist *argstruct)
{
	pid_t pid;
	if ((pid = fork()) < 0)
	{
		fprintf(stderr, "Fork failed\n");
		return -1;
	}
	/* Child runs this */
	if (pid == 0)
	{
		if (argstruct->next == NULL)
		{
			if(strlen(argstruct->output) != 0)
			{
				newfd = open(argstruct->output, O_CREAT | O_WRONLY, 0700);
				dup2(newfd, 1);
			}
			if(strlen(argstruct->input) != 0)
			{
				origfd = open(argstruct->input, O_RDONLY);
			}
			if (origfd != 0)
			{
				dup2(origfd, 0);
			}
			if (execvp(argstruct->argv[0], argstruct->argv) < 0)
			{
				fprintf(stderr, "Exec failed on last stage\n");
				return -1;
			}
		}

		/* Don't need to dup2 read if it's stdin */
		if (origfd != 0)
		{
			dup2(origfd, 0);
			close(origfd);
		}
		/* Don't need to dup2 write if it's stdout */
		if (newfd != 1)
		{
			dup2(newfd, 1);
			close(newfd);
		}

		if (execvp(argstruct->argv[0], argstruct->argv) < 0)
		{
			fprintf(stderr, "Exec failed\n");
			return -1;
		}
	}
	close(origfd);
	close(newfd);
	return 0;
}

int launchfinal(int origfd, int newfd, arglist *argstruct)
{
	pid_t pid;
	if ((pid = fork()) < 0)
	{
		fprintf(stderr, "Fork failed\n");
		return -1;
	}
	/* Child runs this */
	if (pid == 0)
	{
		if(strlen(argstruct->output) != 0)
 	    {
  		      newfd = open(argstruct->output, O_CREAT | O_WRONLY, 0700);
              dup2(newfd, 1);
        }
        if(strlen(argstruct->input) != 0)
        {
              origfd = open(argstruct->input, O_RDONLY);
        }
		if (origfd != 0)
		{
			dup2(origfd, 0);
		}
		if (execvp(argstruct->argv[0], argstruct->argv) < 0)
		{
			fprintf(stderr, "Exec failed on last stage\n");
			return -1;
		}
	}
	wait(NULL);
	/*close(origfd);
	close(newfd);*/
	return 0;
}

int execute(arglist *argstruct)
{
	int argnum;
	int i;
	int origfd = 0;
	int newfd[2];
	argnum = countargs(argstruct);
	for (i = 0; i < argnum; i++)
	{
		if (argstruct->next != NULL)
		{
			pipe(newfd);
			origfd = launchcmd(origfd, newfd[1], argstruct);
			close(newfd[1]);
			if (origfd < 0)
				return -1;
			origfd = newfd[0];
		}
		if (argstruct->next == NULL)
			origfd = launchfinal(origfd, newfd[1], argstruct);
		argstruct = argstruct->next;
	}
	return 0;
}
