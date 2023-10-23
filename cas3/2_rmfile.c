/*
Napisati program koji brise regularni fajl ili direktorijum,
pri cemu je dozvoljeno koristiti samo sistemske pozive.

Napomena:
- funkcija remove() je funkcija standardne C biblioteke
koja radi slicnu stvar, tj. sama proverava da li je prosledjeni
fajl regularan ili direktorijum.

Poziv programa:
./rmfile -[fd] path/to/file/or/directory
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

int main(int argc, char **argv) {
    check_error(3 == argc, "Argument missing. Usage: ./rmfile -[fd] path/to/file/or/directory");
    
    check_error('-' == argv[1][0], "Not a command line option");
    if ('f' == argv[1][1])
		/* brisanje regularnog fajla
		 * TLPI za detalje 
		 */
      check_error(unlink(argv[2]) >= 0, "Removing file failed");
    else if ('d' == argv[1][1])
		/* brisanje direktorijuma
		 * TLPI za detalje 
		 */
      check_error(rmdir(argv[2]) >= 0, "Removing directory failed");
    else 
      check_error(false, "Wrong option, valid are 'f' and 'd'");
    return 0;
}
