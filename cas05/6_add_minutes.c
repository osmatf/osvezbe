/* Ispisati datum i vreme koji se dobijaju kada na trenutno vreme dodamo m minuta.
 * Program kao argument komandne linije dobija broj m i ispisuje na standardni izlaz
 * datum i vreme.
 * 
 * NAPOMENA: isprobati sa primerima gde je m>=60.
 * 
 * Poziv programa:
 * ./a.out m
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <sys/time.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

int main(int argc, char** argv) {
    check_error(argc == 2, "./a.out n");

    int m = atoi(argv[1]);
    time_t now = time(NULL);
    /* Manipulacija vremenom i datumima treba da se radi pomocu localtime uvecavanjem ili
     * smanjivanjem odgovarajucih polja strukture tm. Nije ideja da se polomite komplikovanom
     * matematikom u nadi da cete ubosti ispravno resenje.
     */
    struct tm *brokenTime = localtime(&now);
    check_error(brokenTime != NULL, "localtime");

    printf("Current date and time: %d/%d/%d %d:%d:%d\n", brokenTime->tm_mday, brokenTime->tm_mon + 1, brokenTime->tm_year + 1900, brokenTime->tm_hour, brokenTime->tm_min, brokenTime->tm_sec);

    /* Pomeramo vreme za m godina.
     * NAPOMENA: sta ako je m >= 60? Strukutura ce u polju tm_min imati broj
     * koji je >=60, sto nije validno vreme. Medjutim, kada se pozove funkcija
     * mktime, struktura se normalizuje i dobija se ispravno vreme.
     * 
     * Dakle, ako je u strukturi upisano 1h 50min i na to dodamo 20min, nakon
     * poziva mktime ce u strukturi biti upisano 2h 10min. */
    brokenTime->tm_min += m;
    time_t new_time = mktime(brokenTime);
    check_error(new_time != -1, "...");

    printf("Updated date and time: %d/%d/%d %d:%d:%d\n", brokenTime->tm_mday, brokenTime->tm_mon + 1, brokenTime->tm_year + 1900, brokenTime->tm_hour, brokenTime->tm_min, brokenTime->tm_sec);

    exit(EXIT_SUCCESS);
}
