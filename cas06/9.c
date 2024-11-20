/*
Kao argument komandne linije zadaje se putanja 
do pomocnog programa koji treba pokrenuti.

Pomocni program treba da ispise svoj prvi argument na standardni izlaz.
Izvorni kod pomocnog programa je dat u 9_pom.c. Potrebno ga je iskompajlirati
pre pokretanja ovog programa i nazvati ga nekim imenom (recimo gcc 9_pom.c -o pom).

Nakon sto pomocni program ispise svoj argument i zavrsi sa radom, nas program
treba da ispise status zavrsetka deteta (pomocnog programa).
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define check_error(cond, msg)\
        do{\
          if(!(cond)){\
            perror(msg);\
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
            exit(EXIT_FAILURE);\
          }\
        }while(0)

int main(int argc, char** argv) {
  check_error(argc == 2, "./a.out pom");

  pid_t childPid = fork();
  check_error(childPid != -1, "fork");

  if(childPid == 0){
    /* dete proces nasledjuje argumente komandne linije od roditelja 
     * pa zna da je argv[1] = pom.
     *
     * Dodatno, postujemo praksu da je drugi argument u exec funkcijama
     * naziv programa koji se pokrece (a ne njegova putanja).
     */
    char *last_slash = strrchr(argv[1], '/');
    char *program_name = (last_slash == NULL) ? argv[1] : (last_slash + 1);

    check_error(-1 != execl(argv[1], program_name, "123", NULL), "execl");
  }

  int status;
  check_error(-1 != wait(&status), "wait");

  if(WIFEXITED(status)){
    int exit_code = WEXITSTATUS(status);

    if(exit_code == 0){
      printf("Child finished successfully\n");
    } else {
      printf("[ERROR] EXIT_CODE = %d\n", exit_code);
    }
  } else {
    printf("Child finished abnormally\n");
  }

	exit(EXIT_SUCCESS);
}
