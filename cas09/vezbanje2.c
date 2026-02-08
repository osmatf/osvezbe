/*
Napisati program koji u beskonacnoj petlji ceka da korisnik
unese naziv fajla, a potom ceka na signal.

Ukoliko stigne SIGUSR1, potrebno je ispisati godinu poslednje modifikacije
unetog fajla.
Ukoliko stigne SIGUSR2, potrebno je ispisati naziv vlasnika tog fajla.

Program izlazi iz beskonacne petlje kada stigne SIGQUIT, ispisuje poruku
"Zavrsena obrada signala" i zavrsava sa radom.

NAPOMENA: kako testirati?
Otvoriti jedan terminal i pokrenuti program, npr. ./a.out

Dok se on izvrsava u jednom terminalu, otvoriti drugi, naci PID prvog
procesa sa `ps aux` i slati mu signale koriscenjem komande `kill`, npr.
`kill -SIGUSR1 <PID>`.

U prvom terminalu je potrebno unositi nazive fajlova pre nego sto se posalje signal
iz drugog terminala.
 */
#define _DEFAULT_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <stdint.h>
#include <time.h>
#include <pwd.h>

#define UNUSED_FUNC_ARG(x) ((void)x)
#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

#define MAX_LEN 1024

static bool shouldTerminate = false;
static bool sigusr1 = false;
static bool sigusr2 = false;

/* kompleksnu obradu signala ostavljamo za main funkciju */
void osHandleSIG(int signum){
    switch(signum){
        case SIGUSR1:
            sigusr1 = true;
            break;
        case SIGUSR2:
            sigusr2 = true;
            break;
        case SIGQUIT:
            shouldTerminate = true;
            break;
    }
}

int main(int argc, char **argv) {
    check_error(argc == 1, "argc");

    check_error(SIG_ERR != signal(SIGUSR1, osHandleSIG), "Setting SIGINT handler failed");
    check_error(SIG_ERR != signal(SIGUSR2, osHandleSIG), "Setting SIGTERM handler failed");
    check_error(SIG_ERR != signal(SIGQUIT, osHandleSIG), "Setting SIGTERM handler failed");

    char filename[MAX_LEN];
    while (1) {
        /* citamo naziv fajla sa standardnog ulaza */
        scanf("%s", filename);

        /* cekamo na signal*/
        pause();

        if (shouldTerminate) {
            break;
        }

        /* radimo stat da bismo izvukli neophodne informacije */
        struct stat fileinfo;
        check_error(stat(filename, &fileinfo) != -1, "stat");

        if (sigusr1) {
            /* razbijamo vreme na citljive vrednost */
            struct tm *brokenTime = localtime(&fileinfo.st_mtime);
            check_error(brokenTime != NULL, "localtime");

            printf("%d\n", brokenTime->tm_year + 1900);

            /* vracamo sigusr1 na false */
            sigusr1 = false;
        }

        if(sigusr2){
            /* citamo informacije o vlasniku fajla */
            struct passwd *user_info = getpwuid(fileinfo.st_uid);
            check_error(user_info != NULL, "getpwuid");

            printf("%s\n", user_info->pw_name);

            sigusr2 = false;
        }
    }

    printf("Zavrsena obrada signala\n");

    return 0;
}
