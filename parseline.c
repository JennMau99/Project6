/* Jennifer Maunder and Richa Gadgil
 * CPE 357-03
 * Project 5
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

/* getline takes char array of 512, int startpipe, int endpipe */
int getline(char *array, int stage, int startpipe, int endpipe);

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

		/*printf("input %s", find->input);
		printf("output %s", find->output);
		printf("argc %d", find->argc);
		*/find = find->next;

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
	/*fprintf(stdout, "stage %d: %s\n", stagenum, stage); */
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
	status = getline(stage, stagenum, startpipe, endpipe);

	return status;
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
	fprintf(stdout, "line: ");
	fgets(line, 513, stdin);
	if (line[512] != 0)
	{
		fprintf(stderr, "cmd: too many arguments\n");
		return -1;
	}
	length = strlen(line);
	if (length == 0)
	{
		fprintf(stderr, "cmd: no commands given\n");
		return -1;
	}
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

int getline(char *array, int stage, int startpipe, int endpipe)
{
        int i = 0;
        int sp = startpipe;
        int ep = endpipe;
        int geti = 0;
        int geto = 0;

        char input[512];
        char output[512];

        int argc = 0;
        char argv[10][512];

        char * words;	

	/* head * arglist */
	/*char **argv1 = malloc(10 * sizeof(char*));
	*/
	int wc = 0;
	arglist * find;
	arglist * new_node = (arglist *)malloc(sizeof(arglist));
	new_node->argv = malloc(10 * sizeof(char*));

	/*new_node->input = (char *)malloc(512 * sizeof(char));
	new_node->output = (char *)malloc(512 * sizeof(char));	
	*/
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
	/*new_node->argv = argv1;
	*/
		/*
        printf("\n--------");
        printf("\nStage %d: \"%s\"", stage, array);
        printf("\n--------");
		*/

        words = strtok(array, " ");


        while(words != NULL)
        {

          
		if(strcmp(words, "<") && strcmp(words, ">") && geto == 0 && geti == 0)
                {
                        if(argc > 10)
			{
				fprintf(stderr, "too many arguments!");
				return -1;
			}

			strcpy(argv[argc], words);
                        new_node->argv[argc] = words;
			/*get commands*/
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
			strcpy(new_node->input, words);
                        strcpy(output, words);
                        geto = 0;
                        ep = -2;
                }


                words = strtok(NULL, " ");
        }/*

        printf("\n\tinput: ");
        if(sp == -1)
                printf("original stdin");
        else if(sp == -2)
                printf(input);
        else
                printf("pipe from stage %d", sp);

        printf("\n\toutput: ");
	if(ep == -1)
                printf("original stdout");
        else if(ep == -2)
                printf(output);
        else
                printf("pipe to stage %d", ep);


        printf("\n\targc: %d", argc);
        printf("\n\targv: ");
        for(i = 0; i < argc; i++)
        {
                printf("\"%s\"", argv[i]);
                if(i != (argc - 1))
                        printf(", ");
        }

        printf("\n");
		*/
/*
	printlist();
*/	
        return 0;


}

/* We probably shouldn't have main doing too much */
int main(int argc, char *argv[])
{
	int status = 0;
	char line[513] = {0};
	if (argc > 1)
	{
		fprintf(stderr, "parseline itself doesn't take args\n");
		return -1;
	}
	status = readline(line);
	if (status == 0)
		execute(head);
	return 0;
}

