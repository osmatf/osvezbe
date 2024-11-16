/* Ispisati koji je dan u nedelji bio n godina od sadasnjeg trenutka.
 * (n moze biti i negativno). Program kao argument komandne linije dobija broj
 * n i ispisuje na standardni izlaz dan u nedelji (na srpskom).
 * 
 * Poziv programa:
 * ./a.out n
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

void ispisi_dan_u_nedelji(int tm_wday){
    switch(tm_wday){
        case 1:
            printf("Ponedeljak\n");
            break;
        case 2:
            printf("Utorak\n");
            break;
        case 3:
            printf("Sreda\n");
            break;
        case 4:
            printf("Cetvrtak\n");
            break;
        case 5:
            printf("Petak\n");
            break;
        case 6:
            printf("Subota\n");
            break;
        case 0:
            printf("Nedelja\n");
            break;
    }
}

int main(int argc, char** argv) {
    check_error(argc == 2, "./a.out n");

    int n = atoi(argv[1]);
    time_t now = time(NULL);

    /* Manipulacija vremenom i datumima treba da se radi pomocu localtime uvecavanjem ili
     * smanjivanjem odgovarajucih polja strukture tm. Nije ideja da se polomite komplikovanom
     * matematikom u nadi da cete ubosti ispravno resenje.
     */
    struct tm *brokenTime = localtime(&now);
    check_error(brokenTime != NULL, "localtime");

    /* pomeramo vreme za n godina */ 
    brokenTime->tm_year += n;
    /* funkcija mktime omogucava dobijanje broja sekundi od epohe na osnovu
     * "razbijenog" vremena.
     *
     * NAPOMENA: funkcije za rad sa vremenom koriste staticki alociranu memoriju,
     * tako da svi pozivi ovih funkcija odrzavaju jednu istu strukturu tm koju nam vraca localtime.
     * U ovom slucaju, poziv mktime, osim sto vraca broj sekundi koji predstavlja novo vreme,
     * azurira i polja tm_wday i tm_yday iz strukture tm, na koju mi vec imamo pokazivac brokenTime.
     * Dakle, nije neophodno da ponovo pozivamo localtime (ali nije ni pogresno).
     */ 
    time_t new_time = mktime(brokenTime);
    check_error(new_time != -1, "...");

    /* struktura tm se azurirala tako da mozemo koristit pokazivac koji smo vec dobili od localtime funkcije */
    ispisi_dan_u_nedelji(brokenTime->tm_wday);

    exit(EXIT_SUCCESS);
}
