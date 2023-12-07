/*
Program postavlja prava pristupa 'permissions' fajlu na putanji 
'filePath'. Prava se zadaju u oktalnom formatu.

Poziv programa:
./a.out filePath permissions
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)
	
// ./chmod file prava
int main(int argc, char** argv) {
	
	check_error(argc == 3, "./chmod file permissions");
	
	//prava se zadaju u oktalnom formatu, pa je potrebno ucitati ih u osnovi 8
	int prava = strtol(argv[2], NULL, 8);
	
	//kreira se fajl
	int fd = open(argv[1], O_CREAT, prava);
	check_error(fd != -1, "open");
	
	close(fd);
	
	//menjaju mu se prava pristupa
	check_error(chmod(argv[1], prava) != -1, "chmod");
	
	exit(EXIT_SUCCESS);
}
