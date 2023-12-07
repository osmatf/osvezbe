/*
 * Ovaj primer demonstrira koncept signala. Oni predstavljaju najstariji oblik komunikacije između procesa.
 * 
 * Kada se signal desi kernel operativnog sistema pokreće registrovanu funkciju koja treba da obradi signal.
 * Ako ovakva funkcija nije postavljena i podrazumevano ponašanje nije da se signal ignoriše, aplikacija će
 * završiti izvršavanje nasilno sa exit kodom 128+BROJ_SIGNALA. Na primer SEGFAULT signal je broj 11 dakle
 * aplikacija završava sa exit kodom 128+11=139.
 * 
 * Dakle signal prekida trenutno izvršavanje programa ma šta on radio. Postavlja se prvo pitanje šta treba da
 * se radi nakon obrade signala kada opet stigne isti signal? Da li ponovo zvati postavljenu funkciju za obradu
 * signala ili preduzeti podrazumevanu akciju? Prvobitno je bilo napravljeno da se ponašanje ponovo podešava
 * na podrazumevano što je problem jer može da se desi da dođe do signala pre nego što aplikacija uspe ponovo da
 * postavi funkciju za obradu tog signala. Drugo pitanje je šta se dešava ako se isti signal desi u toku njegove
 * obrade? Da li dopustiti praktično rekurzivan poziv funkcije za obradu signala ili blokirati signal? Druga
 * opcija je bolja iz mnogo razloga. Ova dva pitanja su dovela do dve semantike obrade signala:
 * 1) restartuje se na podrazumevano ponašanje i signali se ne blokiraju dok traje obrada tekućeg signala - ovo
 * ponašanje najčešće NEĆEMO
 * 2) ne restartuje se na podrazumevano ponašanje i koristi se postavljena funkcija; blokiraju se dolasci istog
 * signala za vreme obrade u okviru funkcije za obradu signala - ovo ponašanje ŽELIMO
 * 
 * Zbrka oko semantike je rešena uvođenjem sigaction() poziva kojim se može eksplicitno naglasiti kakvo tačno
 * ponašanje želimo. Izbegli smo da koristimo sigaction() zbog relativno komplikovanog interfejsa.
 * 
 * Poslednja bitna stvar je da nije bezbedno zvati sve funkcije tokom obrade signala. Na primer printf() nije
 * bezbedno zvati (u primeru se koristi samo za demonstraciju). Nije bezbedno jer u toku izvršavanje printf()
 * funkcije unutar funkcije za obradu signala, može da se desi da dođe do novog signala što prekida printf() i
 * ostavlja podatke u nekonzistentnom stanju. Spisak bezbednih funkcija možete dobiti sa 'man 7 signal-safety'.
 * Ukoliko je to moguće, obradu signala treba svesti na postavljanje vrednosti jedne celobrojne promenljive koja
 * bi trebalo da signalizira koji signal se desio i obradu bi trebalo pomeriti van funkcije za obradu signala.
 *
 */
#define _DEFAULT_SOURCE /* NAPOMENA: bez ovoga signal handler se vraca na podrazumevani nakon prvog pristizanja signala. */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <stdint.h>

#define UNUSED_FUNC_ARG(x) ((void)x)

/* makro za proveru gresaka */
#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

static bool os_ShouldTerminate = false;

void osHandleSIGTERM(int signum) {
    /* printf() nije bezbedan za upotrebu unutar signal handler-a ali ga na kursu koristimo zbog jednostavnosti */
    UNUSED_FUNC_ARG(signum);
    printf("Hello from SIGTERM handler, we will terminate after this handler!\n");
    os_ShouldTerminate = true;
}

void osHandleSIGINT(int signum) {
    UNUSED_FUNC_ARG(signum);
    printf("Hello from SIGINT handler!\n");
}

void osHandleSIG(int signum){
    switch(signum){
        case SIGINT:
            printf("SIGINT arrived!\n");
            break;
        case SIGTERM:
            printf("SIGTERM arrived!\n");
            os_ShouldTerminate = true;
            break;
    }
}

int main() {
    printf("1) Press Ctrl+C and see what happens.\n");
    printf("2) Run command 'kill -SIGTERM 9973' (9973 is PID). This command will send signal SIGTERM "
           "and that will invoke signal handler 'osHandleSIGTERM' which will "
           "set 'os_ShouldTerminate' to true and program will end execution.\n");
    printf("===============================================================================\n");
    printf("PID=%jd\n", (intmax_t)getpid());
    
    /**
     * Prvo postavljamo signal hadnler-e
     * */
    check_error(SIG_ERR != signal(SIGINT, osHandleSIGINT), "Setting SIGINT handler failed");
    check_error(SIG_ERR != signal(SIGTERM, osHandleSIGTERM), "Setting SIGTERM handler failed");
    
    /*
     * Takodje mozemo postaviti jedan signal handler za
     * vise razlicitih signala jer handler dobija broj signala
     * kao argument.
     *   check_error(SIG_ERR != signal(SIGINT, osHandleSIG), "Setting SIGINT handler failed");
     *   check_error(SIG_ERR != signal(SIGTERM, osHandleSIG), "Setting SIGTERM handler failed");
    */

    do {
        /**
         * Program ceka na signal
         * */
        printf("Waiting before pause().\n");
        pause();
        
        /**
         * Deo nakon poziva pause() se izvrsava nakon sto stigne signal i signal handler odradi svoj posao
         * */
        printf("After pause().\n");
    } while (!os_ShouldTerminate);

    return 0;
}
