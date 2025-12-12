/*
Program koji ispisuje informacije o svim korisnicima na sistemu.
*/
#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <pwd.h>

/* Makro pomocu koga mozemo da prevarimo kompajler da ne prijavljuje upozorenja
 * za neiskoriscene promenljive (gcc 2_listUsers.c -Wall -Wextra).
 */
#define UNUSED(x) ((void)x)

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

int main(int argc, char **argv)
{
    check_error(argc == 1, "./a.out");
    /* Varamo kompajler da ne bi prijavio upozorenje */
    UNUSED(argv);

    /* Obavezno inicijalizovati pokazivace! */
    struct passwd *currentUser = NULL;

    /* Prilikom citanja sadrzaja passwd fajla neophodno je na pocetku
     * premotati ceo fajl na pocetak pozivom funkcije setpwent().
     *
     * Nema potrebe obradjivati greske za setpwent jer poziv ne puca.
     */
    setpwent();

    /* Passwd fajl se iterativno cita liniju po liniju pomocu funkcije
     * getpwent().
     *
     * Funkcija vraca NULL ako dodje do kraja fajla ili ako se dogodi greska.
     * Za proveru gresaka mora da se koristi errno promenljiva.
     * U ovom zadatku cemo taj deo preskociti.
     */
    while ((currentUser = getpwent()) != NULL) {

        /* Stampamo informacije o korisniku */
        printf("User name: %s\n", currentUser->pw_name);
        printf("Numerical user id: %jd\n", (intmax_t)currentUser->pw_uid);
        printf("Numerical group id: %jd\n", (intmax_t)currentUser->pw_gid);
        printf("Comment: %s\n", currentUser->pw_gecos);
        printf("Home directory: %s\n", currentUser->pw_dir);
        printf("Shell: %s\n", currentUser->pw_shell);
    }

    /* Nakon sto se zavrsi upotreba passwd fajl MORA da se pravilno
     * zatvori pozivom funkcije endpwent().
     */
    endpwent();

    exit(EXIT_SUCCESS);
}
