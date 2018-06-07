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
#include <dirent.h>

/* Frees everything in the arglist struct. */
void freehead()
{
	int a = 0;
	arglist *nextstruct;
	arglist *args = head;
	while(args)
	{
		nextstruct = args->next;
		a = 0;
		while( args->argv[a] != NULL)
		{
			free(args->argv[a]);
			a++;
		}
		free(args->argv[a]);
		free(args->argv);
		free(args);
		args = nextstruct;
	}
	head = NULL;
}
/* Counts the number of args in an arglist struct. */
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
/* If an execvp errors, checks what was the cause of the
   error and prints it. */
int checkerror(arglist *argstruct)
{
	DIR *dir = opendir(argstruct->argv[0]);
	if (access(argstruct->argv[0], F_OK) == -1 && dir == NULL)
	{
		fprintf(stderr, "%s: No such file or directory\n", argstruct->argv[0]);
		return 0;
	}
	if (dir)
		closedir(dir);
	fprintf(stderr, "%s: Permission denied\n", argstruct->argv[0]);
	return 0;
}
/* Forks and then executes a process in the child. */
int launchcmd(int origfd, int newfd, arglist *argstruct)
{
	pid_t pid;
	int status = 0;
	if ((pid = fork()) < 0)
	{
		fprintf(stderr, "Fork failed\n");
		exit(0);
		return -1;
	}
	/* Child runs this */
	if (pid == 0)
	{
		/* Don't need to dup2 read if it's stdin */
		if (origfd != 0)
		{
			dup2(origfd, 0);
		}
		/* Don't need to dup2 write if it's stdout */
		if (newfd != 1)
		{
			dup2(newfd, 1);
		}

		if (execvp(argstruct->argv[0], argstruct->argv) < 0)
		{
			checkerror(argstruct);
			exit(EXIT_FAILURE);
			return -1;
		}
	}
	wait(&status);
	if (origfd != 0)
		close(origfd);
	close(newfd);
	if (WIFEXITED(status))
	{
		if (WEXITSTATUS(status) != 0)
			return -1;
	}
	return 0;
}
/* Forks and then executes a process in the child-
   only runs on the final stage. */
int launchfinal(int origfd, int newfd, arglist *argstruct)
{
	pid_t pid;
	int status = 0;
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
		/*close(newfd);*/
		if (execvp(argstruct->argv[0], argstruct->argv) < 0)
		{
			checkerror(argstruct);
			exit(EXIT_FAILURE);
			return -1;
		}
	}
	wait(&status);
	if (origfd != 0)
		close(origfd);
	if (newfd != 1)
		close(newfd);
	if (WIFEXITED(status))
	{
		if (WEXITSTATUS(status) != 0)
			return -1;
	}
	return 0;
}
/* Loops through all of the argstructs and executes the
   commands for each of them. */
int execute(arglist *argstruct)
{
	int argnum;
	int i;
	int origfd = 0;
	int newfd[2];
	argnum = countargs(argstruct);
	if (argstruct->next == NULL)
	{
		origfd = launchfinal(origfd, 1, argstruct);
		freehead();
		return 0;
	}
	for (i = 0; i < argnum; i++)
	{
		if (argstruct->next != NULL)
		{
			pipe(newfd);
			origfd = launchcmd(origfd, newfd[1], argstruct);
			close(newfd[1]);
			if (origfd < 0)
			{
				freehead();
				return -1;
			}
			origfd = newfd[0];
		}
		if (argstruct->next == NULL)
			origfd = launchfinal(origfd, newfd[1], argstruct);
		argstruct = argstruct->next;
	}
	close(newfd[0]);
	fflush(stdout);
	freehead();
	return 0;
}
