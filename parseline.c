/* Jennifer Maunder and Richa Gadgil
 * CPE 357-03
 * Project 5
 */

#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "header.h"
#include <setjmp.h>
#include <sys/stat.h>

/* getline takes char array of 512, int startpipe, int endpipe */
int get_line(char *array, int stage, int startpipe, int endpipe);
sigjmp_buf ctrlc;


void printlist()
{
	int i;
	arglist * find;
	find = head;
	
	while(find != NULL)
	{
		for(i = 0; i < find->argc; i++)
		{
			printf("%s ", find->argv[i]);	
		}	

		printf("\nargc %d", find->argc);
		find = find->next;
	}

}

int changedirectory(char *line)
{
	char *token;
	
	token = strtok(line, " ");
	token = strtok(NULL, " ");
	if (token == NULL || *token == 10)
	{
		return 0;
	}
	if (token[strlen(token) - 1] == '\n')
		token[strlen(token) -1] = '\0';
	
	if (chdir(token) < 0)
	{
		fprintf(stderr, "%s: No such file or directory\n", token);
		return -1;
	}
	return 0;	
}

int checkstage(char *stage, int stagenum, int startpipe, int endpipe)
{
	int status = 0;
	int leftchevron = 0;
	int rightchevron = 0;
	int i;
	int length;
	int notzero = 0;
	length = strlen(stage);
	for (i = 0; i < length; i++)
	{
		if (stage[i] == '<')
			leftchevron++;
		else if (stage[i] == '>')
			rightchevron++;
		if (stage[i] != ' ' && stage[i] != 10)
			notzero = 1;
	}
	if (length == 0 || notzero == 0)
	{
		fprintf(stderr, "Invalid null command\n");
		return -1;
	}
	if (stage[length - 1] == 10)
		stage[length - 1] = '\0';
	if (leftchevron > 1)
	{
		fprintf(stderr, "Bad input redirection\n");
		return -1;
	}
	if (rightchevron > 1)
	{
		fprintf(stderr, "Bad output redirection\n");
		return -1;
	}
	if (leftchevron  && startpipe != -1)
	{
		fprintf(stderr, "Ambiguous Input\n");
		return -1;
	}
	if (rightchevron && endpipe != -1)
	{
		fprintf(stderr, "Ambiguous Output\n");
		return -1;
	}
	status = get_line(stage, stagenum, startpipe, endpipe);

	return status;
}

int inputline(char line[513])
{
	char c;
	int i = 0;
	fprintf(stdout, "8-P ");
	c = getchar();
	if(c == EOF)
	{
		printf("^D\n");
		exit(0);
	}

	while (c != '\n' && c != EOF)
	{
		line[i] = c;
		i++;
		if (i > 511)
			return -1;
		c = getchar();
	}
	line[i++] = c;
	line[i] = '\0';
	return 0;
}

int readline(char *line, int checker)
{
	int i = 0;
	int j = 0;
	int stagenum = 0;
	int startpipe = -1;
	int endpipe = -1;
	int length = 0;
	int check = 0;
	int limit = 0;
	char stage[513] = {0};
	if (checker == 1)
	{
		if(inputline(line) < 0)
		{
			fprintf(stderr, "Too many arguments.\n");
			return -1;
		}
	}
	
	if (strcmp(line, "exit\n") == 0)
		return 2;
	if (strncmp(line, "cd", 2) == 0 && (line[2] == '\n' || line[2] == ' '))
	{
		changedirectory(line);
		return 3;
	}

	length = strlen(line);
	if (length == 0)
	{
		fprintf(stderr, "cmd: no commands given\n");
		return 3;
	}
	if (strcmp(line, "exit") == 0)
		return 2;
	/* band aids are amazing */
	for (i = 0; i < length; i++)
	{	
		if (line[i] == '|')
			j = 1;
	}
	if (j == 0)
		return checkstage(line, 0, -1, -1);
	j = 0;
	for (i = 0; i < length; i++)
	{
		if (line[i] != '|')
			stage[j++] = line[i];
		else if (line[i] == '|')
		{
			if (endpipe == -1)
				endpipe++;
			endpipe++;
			stage[j] = '\0';
			limit++;
			if(limit > 10)
			{
				fprintf(stderr, "Pipeline too deep.\n");
				return -1;
			}
			check = checkstage(stage, stagenum, startpipe, endpipe);
			if (check == -1)
				return -1;
			stagenum++;
			startpipe++;
			j = 0;
		}
	}
	endpipe = -1;
	stage[j-1] = '\0';
	limit++;
	if(limit > 10)
        {
        	fprintf(stderr, "Pipeline too deep.\n");
            return -1;
        }
	check = checkstage(stage, stagenum, startpipe, endpipe);
	if (check == -1)
		return -1;
	return 0;
}
int get_words(char *array)
{
	int wordnum = 0;
	char *wording = (char*)malloc((strlen(array) + 1) * sizeof(char));
	char *words;
	strcpy(wording, array);
	words = strtok(wording, " ");

	while(words != NULL)
	{	
		if(strcmp(words, "<") != 0 && strcmp(words, ">") != 0)
			wordnum++;
		words = strtok(NULL, " ");
	}
	free(wording);
	return wordnum;	

}
int get_line(char *array, int stage, int startpipe, int endpipe)
{
    int sp = startpipe;
    int ep = endpipe;
    int geti = 0;
    int geto = 0;
 
	char input[512];
    char output[512];

    int argc = 0;
    char argv[10][512];

    char * words;	
	int wordnum = 0;

	arglist * find;
	arglist * new_node = (arglist *)malloc(sizeof(arglist));

	memset(new_node, '\0', sizeof(arglist));
	
	wordnum = get_words(array); 
	

	/* To do: You can't malloc a set value here for it to free correctly,
 		so find the number of words + 1 (for NULL) and malloc that */
	/* changed this to 11 so there's room for a NULL at the end */
	new_node->argv = malloc((wordnum + 1) * sizeof(char *));
	


	if(head == NULL)
	{
		head = new_node;
	}
	else 
	{
		find = head;
		while(find->next != NULL)
		{
			find = find->next;	
		}
		find->next = new_node;
	}
	
	new_node->next = NULL;
    



	  words = strtok(array, " ");

   while(words != NULL)
    {     
		if(strcmp(words, "<") && strcmp(words, ">") && geto == 0 && geti == 0)
    	{
			/* Changed this from 512 to the word size */
			new_node->argv[argc] = malloc((strlen(words) + 1) * sizeof(char));
        	if(argc > 10)
			{
				fprintf(stderr, "Too many arguments.\n");
				new_node->argv[argc] = NULL;
				freehead();
				return -1;
			}

			strcpy(argv[argc], words);           
			strcpy(new_node->argv[argc],words);
        	argc++;
    	}	
		
		new_node->argc = argc;
	    if(sp == -1 && strcmp(words, "<") == 0)
		{ 
        	geti = 1;
		}
        else if(geti == 1)
        {
			strcpy(new_node->input, words);			
			strcpy(input, words);
            geti = 0;
            sp = -2;
        }
        if(ep == -1 && strcmp(words, ">") == 0)
        {
			geto = 1;
		}
		else if(geto == 1)
        {
			strcpy(new_node->output, words);
			strcpy(output, words);
            geto = 0;
            ep = -2;
        }
            words = strtok(NULL, " ");
	}
	/* Set the last item in the *char[] to NULL */
	new_node->argv[argc] = NULL;
	return 0;
}

int loop(int argc, char *argv[])
{
	arglist *nextstruct;
	arglist *args;

	int status = 1;
	char line[513] = {0};
	int i = 0;
	setbuf(stdout, NULL);


	while(sigsetjmp(ctrlc, 1) != 0);

	while (1)
	{
		fflush(stdout);
		for (i = 0; i < 513; i++)
			line[i] = 0;
		status = readline(line, 1);
		if (status == 2)
			return 0;
		if (status == 0)
		{
			execute(head);
		}
		args = head;
		while(args)
		{
			nextstruct = args->next;
			free(args->argv);
			free(args);
			args = nextstruct;
		}
		head = NULL;
		
		freehead();
	}
}

void handle(int hi)
{
		printf("\n");
		fflush(stdout);
		siglongjmp(ctrlc, 1);
		return;        
}

/* We probably shouldn't have main doing too much */
int main(int argc, char *argv[])
{
	
	int i = 0;
	int len = 0;
	char *str;
	int status = 1;
	struct stat st;
	FILE *file;
	
	char *line = NULL;
	size_t len1 = 0;
	char read;


	signal(SIGINT, handle);
	

	if (argc > 1)
	{
		if(stat(argv[1], &st) > -1)
		{
			file = fopen(argv[1], "r");
			while ((read = getline(&line, &len1, file)) != -1) 
			{
				status = readline(line, 0);
				
				if (status == 2)
                	return 0;
               	if (status == 0)
               	{
                    execute(head);
                }	

			}	
			return 1;
		}
		for(i = 1; i < argc; i++)
		{
			len += strlen(argv[i]);	
		}
		str = (char *)malloc((len + i + 1) * sizeof(char));

		for(i = 1; i < argc; i++)
        {
            strcat(str, argv[i]);
			if(i != (argc - 1))
				strcat(str, " ");
        }
		strcat(str, "\0");
		status = readline(str, 0);
        if (status == 2)
        	return 0;
        if (status == 0)
        {
        	execute(head);
        }
		free(str);
		return -1;
	}
	loop(argc, argv);
	return 0;
}

