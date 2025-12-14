/*
Ispisati informacije o korisniku cije je ime 'username'.
Ukoliko korisnik ne postoji, ispisati odgovarajuću poruku na
standardni izlaz.

Poziv programa:
./userInfo username
*/

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Neophodno zaglavlje prilikom rada sa funkcijama
 * koje citaju passwd fajl.
 */
#include <pwd.h>

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
    check_error(argc == 2, "./a.out username");

    /* Citamo informacije iz passwd fajla o korisniku sa
     * korisnickim imenom username.
     * Funkcija getpwnam vraca pokazivac na staticki alociranu strukturu.
     *
     * BITNO: Ovu strukturu nikada ne smemo eksplicitno da oslobadjamo.
     * Vise u man stranama.
     */
    errno = 0;
    struct passwd *userInfo = getpwnam(argv[1]);
    /* U slucaju greske ili nepostojanja podataka u passwd fajlu o
     * korisniku sa imenom username funkcija getpwnam vraca NULL
     * i postavlja errno na odgovarajucu vrednost.
     * Da bismo razlikovali ta dva slucaja, moramo proveriti errno.
     */
    if (userInfo == NULL) {
        if (errno == 0) {
            printf("Korisnik sa imenom '%s' ne postoji!\n", argv[1]);
        } else {
            /* sada smo sigurni da je greska, pa prosledjujemo 0 (false) makrou check_error */
            check_error(0, "getpwnam");
        }
    } else {
        /* Ispisujemo informacije koje postoje u passwd fajlu o korisniku */
        printf("User name: %s\n", userInfo->pw_name);
        /*
         * Tip intmax_t predstavlja CELOBROJNI tip koji može da smesti
         * najveću moguću vrednost od svih standardnih celobrojnih tipova.
         *
         * Zašto je poželjno koristiti intmax_t pri ispisu user ID-a (pw_uid):
         *
         * - Tip pw_uid (uid_t) NIJE garantovano istog tipa na svim sistemima.
         *
         * - Format specifikator %jd je standardni način da se ispiše intmax_t.
         *
         * Prednosti ovog pristupa:
         * - prenosivost koda između različitih UNIX/Linux sistema
         * - izbegavanje pretpostavki o veličini i znaku uid_t
         */
        printf("Numerical user id: %jd\n", (intmax_t)userInfo->pw_uid);
        printf("Numerical group id: %jd\n", (intmax_t)userInfo->pw_gid);
        printf("Comment: %s\n", userInfo->pw_gecos);
        printf("Home directory: %s\n", userInfo->pw_dir);
        printf("Shell: %s\n", userInfo->pw_shell);
    }

    exit(EXIT_SUCCESS);
}
