/* Jennifer Maunder
 * CPE 357-03
 * Project 6
 */

struct arglist {
	char input[512];
	char output[512];
	int argc;
	char argv[10][512];
	struct arglist *next;
} ;

typedef struct arglist arglist;

arglist *head;




