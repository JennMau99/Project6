/* Jennifer Maunder
 * CPE 357-03
 * Project 6
 */

typedef struct arglist {
	char *input;
	char *output;
	int argc;
	char **argv;
	struct arglist *next;
} arglist;

int execute(arglist *argstruct);

