/* Jennifer Maunder and Richa Gadgil
 * CPE 357-03
 * Project 6
 */

struct alist {
	char input[512];
	char output[512];
	int argc;
	char **argv;
	struct alist *next;
} alist;

typedef struct alist arglist;

arglist *head;

void freehead();
int execute(arglist *argstruct);
