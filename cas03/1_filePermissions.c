/*
Napisati program koji ispisuje velicinu fajla na
standardni izlaz ukoliko ni vlasnik, ni grupa vlasnik,
a ni ostali nemaju pravo citanja.

Poziv programa:
./a.out <filepath>
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

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
    check_error(argc == 2, "./a.out <filepath>");

    struct stat fileinfo;
    check_error(stat(argv[1], &fileinfo) != -1, "stat");

    /*
     * Informacije o dozvolama (privilegijama) fajla nalaze se u polju
     * st_mode strukture stat.
     *
     * Dozvole pristupa zapisane su kao bit-maska i dele se na tri grupe:
     *  - vlasnik fajla (user)
     *  - grupa fajla (group)
     *  - ostali korisnici (others)
     *
     * Konstantе S_IRUSR, S_IRGRP i S_IROTH označavaju dozvolu za čitanje
     * fajla za vlasnika, grupu i ostale korisnike. Slično i za w i x pravo.
     *
     * Proverom odgovarajućih bitova u st_mode polju može se utvrditi
     * da li fajl ima (ili nema) određenu privilegiju.
     */
    if (!(fileinfo.st_mode & S_IRUSR) && !(fileinfo.st_mode & S_IRGRP) &&
        !(fileinfo.st_mode & S_IROTH)) { /* isto se postize sa if(!(fileinfo & 0444))*/
        printf("Velicina fajla je %jd\n", (intmax_t)fileinfo.st_size);
    }
    return 0;
}
