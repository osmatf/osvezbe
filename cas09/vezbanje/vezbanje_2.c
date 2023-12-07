#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)
    
static const char *os_Usage = "./2 pathToFifo";

int main(int argc, char** argv) {
	
	check_error(argc == 2, os_Usage);
	
	/* otvorimo fifo sa fopen
	 * najlakse je, jer time izbegavamo gimnasticenje sa 
	 * parsiranjem stringa, a i u tekstu ne pise da je zabranjena
	 * upotreba stdio :)
	 */ 
	FILE* f = fopen(argv[1], "r");
	
	check_error(f != NULL, "fopen failed");
	
	float sum = 0;
	float number = 0;
	
	/* dok uspesno konvertujemo vrednosti */
	while (fscanf(f, "%f", &number) == 1) {
		
		/* dodajemo procitanu vrednost na sumu */
		sum += number;
	}
	
	/* ako nismo stigli do kraja fajla znaci da smo naisli na
	 * nesto sto nije broj 
	 */ 
	errno = EINVAL;
	check_error(feof(f), "Not a number");
	
	/* stampamo rezultat */
	printf("%f\n", sum);
	
	/* zatvaramo fajl */
	fclose(f);
	
	exit(EXIT_SUCCESS);
}
