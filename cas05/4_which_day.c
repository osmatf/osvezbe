/* Ispisati koji je dan u nedelji bio n godina od sadasnjeg trenutka.
 * (n moze biti i negativno). Program kao argument komandne linije dobija broj
 * n i ispisuje na standardni izlaz dan u nedelji (na srpskom).
 *
 * Poziv programa:
 * ./a.out n
 */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <time.h>

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

/* Funkcija ispisuje naziv dana u nedelji na osnovu vrednosti tm_wday
 * (0 = nedelja, 1 = ponedeljak, ..., 6 = subota), prema POSIX konvenciji.
 */
void ispisi_dan_u_nedelji(int tm_wday)
{
    switch (tm_wday) {
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

int main(int argc, char **argv)
{
    check_error(argc == 2, "./a.out n");

    int n = atoi(argv[1]);
    time_t now = time(NULL);

    /* Rad sa datumima i vremenom treba obavljati preko funkcije localtime,
     * uz odgovarajuće uvećavanje ili smanjivanje polja strukture tm.
     * Nije cilj ručno računati datume i vremena složenim formulama,
     * već koristiti postojeću funkcionalnost standardne biblioteke.
     */
    struct tm *brokenTime = localtime(&now);
    check_error(brokenTime != NULL, "localtime");

    /* pomeramo vreme za n godina */
    brokenTime->tm_year += n;
    /* Funkcija mktime pretvara "razbijeno" vreme (struct tm) u broj sekundi
     * proteklih od epohe (time_t).
     *
     * NAPOMENA: Funkcije za rad sa vremenom koriste statički alociranu strukturu tm.
     * Zbog toga svi pozivi funkcije localtime vraćaju pokazivač na istu strukturu.
     *
     * U ovom slučaju, poziv mktime ne samo da vraća novi broj sekundi,
     * već i ažurira određena polja strukture tm (npr. tm_wday i tm_yday)
     * na koju pokazuje promenljiva brokenTime.
     * Zbog toga nije neophodno ponovo pozivati localtime,
     * iako bi takav poziv bio ispravan.
     */
    time_t new_time = mktime(brokenTime);
    check_error(new_time != -1, "...");

    /* struktura tm se azurirala tako da mozemo koristit pokazivac koji smo vec dobili od localtime
     * funkcije */
    ispisi_dan_u_nedelji(brokenTime->tm_wday);

    exit(EXIT_SUCCESS);
}
