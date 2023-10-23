/*
Napisati program koji kreira direktorijum sa imenom
koje se zadaje kao argument komandne linije, pri cemu
za prava pristupa tom direktorijimu koristiti 0755.

Poziv programa:
./a.out dir_name
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

/*
Funkcija kreira direktorijum na prosledjenoj putanji.
Funkcija vraca false u slucaju neuspeha, pa greska
moze biti obradjena u main-u.
Gresku smo mogli da obradimo i u samoj funkciji, pri cemu
tada ne bi imalo potrebe vracati bool.
*/
bool os_create_directory(const char *dirPath) {
  /* privilegije sa kojima se kreira direktorijum */
  static mode_t mode = 0755;
  /* kreiranje direktorijuma
   * TLPI za detalje
   */
  return mkdir(dirPath, mode) >= 0;
}

int main(int argc, char **argv) {
    check_error(2 <= argc, "Argument missing");
    check_error(os_create_directory(argv[1]), "Create directory failed");
    return 0;
}
