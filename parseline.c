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
#include "header.h"

/* getline takes char array of 512, int startpipe, int endpipe */
int get_line(char *array, int stage, int startpipe, int endpipe);

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
		fprintf(stderr, "invalid null command\n");
		return -1;
	}
	if (stage[length - 1] == 10)
		stage[length - 1] = '\0';
	if (leftchevron > 1)
	{
		fprintf(stderr, "cmd: bad input redirection\n");
		return -1;
	}
	if (rightchevron > 1)
	{
		fprintf(stderr, "cmd: bad output redirection\n");
		return -1;
	}
	if (leftchevron  && startpipe != -1)
	{
		fprintf(stderr, "cmd: ambiguous input\n");
		return -1;
	}
	if (rightchevron && endpipe != -1)
	{
		fprintf(stderr, "cmd: ambiguous output\n");
		return -1;
	}
	status = get_line(stage, stagenum, startpipe, endpipe);

	return status;
}

int inputline(char line[513])
{
	char c;
	int i = 0;
	fprintf(stdout, "line: ");
	c = getchar();
	while (c != '\n' && c != EOF)
	{
		line[i] = c;
		i++;
		if (i > 511)
		{
			printf("you shouldn't be here");
			return -1;
		}
		c = getchar();
	}
	line[i++] = c;
	line[i] = '\0';
	return 0;
}

int readline(char *line)
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
	if (inputline(line) < 0)
	{
		fprintf(stderr, "cmd: too many arguments\n");
		return -1;
	}
	if (strcmp(line, "exit\n") == 0)
		return 2;
	length = strlen(line);
	if (length == 0)
	{
		fprintf(stderr, "cmd: no commands given\n");
		return -1;
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
				fprintf(stderr, "too many commands!");
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
        	fprintf(stderr, "too many commands!");
            return -1;
        }
	check = checkstage(stage, stagenum, startpipe, endpipe);
	if (check == -1)
		return -1;
	return 0;
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

	arglist * find;
	arglist * new_node = (arglist *)malloc(sizeof(arglist));
	memset(new_node, '\0', sizeof(arglist));

	new_node->argv = malloc(10 * sizeof(char *));
	
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

	fprintf(stderr, new_node->input);
    while(words != NULL)
    {     
	if(strcmp(words, "<") && strcmp(words, ">") && geto == 0 && geti == 0)
        {
			new_node->argv[argc] = malloc(512 * sizeof(char));

            if(argc > 10)
			{
				fprintf(stderr, "too many arguments!");
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
    return 0;
}

int loop(int argc, char *argv[])
{
	int status = 1;
	char line[513] = {0};
	int i = 0;
	setbuf(stdout, NULL);
	while (1)
	{
		fflush(stdout);
		for (i = 0; i < 513; i++)
			line[i] = 0;
		status = readline(line);
		if (status == 2)
			return 0;
		if (status == 0)
			execute(head);
	}
}



void handle(int hi)
{

        printf("u didnt do it right, try ^D");
        fflush(stdout);

}

/* We probably shouldn't have main doing too much */
int main(int argc, char *argv[])
{
	

	signal(SIGINT, handle);
	if (argc > 1)
	{
		fprintf(stderr, "parseline itself doesn't take args\n");
		return -1;
	}
	loop(argc, argv);
	return 0;
}

