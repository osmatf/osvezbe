/*
Primeri makroa za obradu gresaka koje cemo koristiti
na kursu. Ideja je za sitnije provere izbeci dodatne
funkcijske pozive.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
Marko check_error ispisuje poruku msg na standardni
izlaz za greske u slucaju neispunjenosti uslova cond.

check_error ne koristi funkcijski poziv, 
stoga su neophodne zagrade za cond (probati
bez zagrada).

__FILE__, __func__ i __LINE__ su kompajlerski definisani makroi
koji redom oznacavaju fajl, funkciju i liniju u kojoj se desila greska
*/
#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)


/*
Makro osAssert ispisuje poruku msg na standardni
izlaz za greske u slucaju neispunjenosti uslova cond,
kao i ime fajla i broj linije u kojoj je nastala greska.
Za razliku od check_error makroa, ovaj makro koristi funkcijski poziv.
*/
#define os_assert(cond, msg) os_error_fatal(cond, msg, __FILE__, __LINE__)

void os_error_fatal(bool cond, const char *msg, const char *file, int line)
{
   if (!cond) {
     perror(msg);
     fprintf(stderr, "%s:%d\n", file, line);
     exit(EXIT_FAILURE);
   }
}

int main(){
  FILE *ulaz= fopen("ulaz.txt","r");

  check_error(ulaz != NULL, "Otvaranje datoteke nije uspelo");
  //os_assert(ulaz != NULL, "Otvaranje datoteke nije uspelo");

  return 0;
}