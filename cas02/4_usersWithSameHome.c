/*
Napisati funkciju koja proverava da li dva korisnika imaju isti home direktorijum.
Napisati i program koji za korisnike cija se imena prosledjuju kao argumenti
komandne linije ispisuje da li imaju isti home direktorijum.

Pretpostaviti da ce zadati korisnici postojati na sistemu.

Poziv programa:
./a.out username1 username2
*/
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * Funkcija proverava da li dva korisnika imaju isti home direktorijum.
 *
 * Pošto `getpwnam` koristi zajedničku memoriju, home direktorijum prvog korisnika se
 * kopira pre drugog poziva.
 *
 * Parametri:
 *  - username1: ime prvog korisnika
 *  - username2: ime drugog korisnika
 *
 * Povratna vrednost:
 *  - 1 ako korisnici imaju isti home direktorijum, 0 inače.
 */
int is_same_home(const char *username1, const char *username2)
{
    /**
     * Sve funkcije koje rade sa passwd strukturom koriste isto
     * parce memorije gde upisuju podatke, odnosno dele jednu strukturu
     * (videti man getpwnam). Zbog toga ce drugi poziv funkcije getpwnam
     * pregaziti podatke na koje pokazuje user_info1.
     *
     * */
    struct passwd *user_info1 = getpwnam(username1);
    check_error(user_info1 != NULL, "getpwnam");

    /* Prepisujemo home direktorijum u lokalnu promenljivu*/
    char *user1_home_dir = malloc(strlen(user_info1->pw_dir) + 1);
    check_error(user1_home_dir != NULL, "malloc");

    strcpy(user1_home_dir, user_info1->pw_dir);

    struct passwd *user_info2 = getpwnam(username2);
    check_error(user_info2 != NULL, "getpwnam");

    return (strcmp(user1_home_dir, user_info2->pw_dir) == 0);
}

/**
 * Pogresna verzija funkcije is_same_home. Isprobati ovu funkciju u main-u
 * i proveriti sta ona vraca (uvek indikator da su isti direktorijumi).
 */
int is_same_home_incorrect(const char *username1, const char *username2)
{
    struct passwd *user_info1 = getpwnam(username1);
    check_error(user_info1 != NULL, "getpwnam");

    struct passwd *user_info2 = getpwnam(username2);
    check_error(user_info2 != NULL, "getpwnam");

    return (strcmp(user_info1->pw_dir, user_info2->pw_dir) == 0);
}

int main(int argc, char **argv)
{
    check_error(argc == 3, "./a.out username1 username2");

    if (is_same_home(argv[1], argv[2])) {
        printf("%s i %s imaju isti home direktorijum\n", argv[1], argv[2]);
    } else {
        printf("%s i %s nemaju isti home direktorijum\n", argv[1], argv[2]);
    }

    exit(EXIT_SUCCESS);
}
