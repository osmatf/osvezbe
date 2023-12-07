/*
Napisati program koji fajlu cija se putanja zadaje
kao argument komandne linije ukida pravo citanja
za ostale i dodaje pravo pisanja za grupu.

Primedba:
- za ovakve operacije nad fajlom sam fajl ne otvaramo

Poziv programa:
./a.out filePath
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
	check_error(argc == 2, "./chmod file ");

	/* izdvajamo stara prava pristupa fajlu*/
	struct stat fileinfo;
	check_error(stat(argv[1], &fileinfo) != -1, "stat");
	mode_t current_mode = fileinfo.st_mode;

	/* dodajemo i oduzimamo trazena prava*/
	mode_t new_mode = (current_mode | S_IWGRP) & ~S_IROTH;
	
	/* menjaju mu se prava pristupa */
	check_error(chmod(argv[1], new_mode) != -1, "chmod");
	
	exit(EXIT_SUCCESS);
}
