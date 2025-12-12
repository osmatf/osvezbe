/*
Ispisati nazive home direktorijuma svih korisnika na sistemu cije ime pocinje sa
zadatim prefiksom.

Napomena: ukoliko je putanja do home direktorijuma /home/filip, njegov naziv je filip.

Poziv programa:
./a.out <prefix>
*/
#define _XOPEN_SOURCE 700

#include <errno.h>
#include <pwd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define UNUSED(x) ((void)x)

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

/**
 * Funkcija proverava da li string `name` počinje prefiksom `prefix`.
 *
 * Parametri:
 *  - name:   string koji proveravamo
 *  - prefix: string koji treba da se pojavi na početku `name`
 *
 * Povratna vrednost:
 *  - 1 ako string `name` počinje sa `prefix`
 *  - 0 u suprotnom slučaju
 *
 * Primeri:
 *  starts_with("hello", "he")   → 1
 *  starts_with("hello", "hi")   → 0
 *  starts_with("cat", "cats")   → 0
 *
 * Napomene:
 *  - Oba stringa se moraju završiti terminirajućom nulom.
 *  - Funkcija pretpostavlja da pokazivači nisu NULL.
 *  - Funkcija je implementirana pokazivačkom aritmetikom
 * da bismo vežbali pokazivače.
 */
int starts_with(const char *name, const char *prefix)
{

    while (*prefix != '\0') {
        if (*prefix != *name) {
            return 0;
        }

        prefix++;
        name++;
    }

    return 1;
}

int main(int argc, char **argv)
{
    check_error(argc == 2, "./a.out prefix");

    /* Obavezno inicijalizovati pokazivace! */
    struct passwd *currentUser = NULL;

    setpwent();

    /* Da bismo proverili gresku za poziv getpwent(), pre svakog
     * njegovog poziva moramo postaviti errno na 0.
     */
    errno = 0;
    while ((currentUser = getpwent()) != NULL) {
        if (starts_with(currentUser->pw_name, argv[1])) {
            printf("%s: %s\n", currentUser->pw_name, currentUser->pw_dir);
        }

        errno = 0;
    }
    check_error(errno == 0, "getpwent");

    endpwent();

    exit(EXIT_SUCCESS);
}
