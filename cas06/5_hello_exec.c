/* exec je sistemski poziv koji omogucava izvrsavanje drugih programa. 
Fork vrsi kopiranje adresnog prostora, exec vrsi zamenu adresnog prostora.
Nakon exec-a sadrzaj adresnog prostora se prepisuje novim procesom.

U ovom primeru, cilj je pokrenuti komandu "ls" sa argumentom "-l".
*/ 
#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <sys/wait.h>
#include <string.h>

/* makro za proveru gresaka */
#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

int main(int argc, char** argv) {
	
  /* Procitati u TLPI deo 27.2 (The exec() library functions)*/
	check_error(execlp("ls", "ls", "-l", NULL) != -1, "exec failed");
	
  printf("Ovaj deo koda se ne izvrsava\n");

	exit(EXIT_SUCCESS);
}
