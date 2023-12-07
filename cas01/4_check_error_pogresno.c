/*
Primer cestih gresaka na ispitu pri koriscenju makroa.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
3 ceste greske:
- zagrade oko promenljive cond
- zaboravljen karakter '\' na kraju nekog od redova
- beline nakon karaktera '\', a pre novog reda
*/
#define check_error1(cond,msg)\
  do {\
    if (!cond) {\
      perror(msg);\
      fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

#define check_error2(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);
      exit(EXIT_FAILURE);\
    }\
  } while (0)

#define check_error3(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);\ 
      exit(EXIT_FAILURE);\
    }\
  } while (0)

int main(){
  FILE *ulaz= fopen("ulaz.txt","r");

  check_error1(ulaz != NULL, "Otvaranje datoteke nije uspelo");
  check_error2(ulaz != NULL, "Otvaranje datoteke nije uspelo");
  check_error3(ulaz != NULL, "Otvaranje datoteke nije uspelo");

  return 0;
}