/* Ispisati ukupnu velicinu direktorijuma cija se putanja zadaje kao
 * argument komandne linije.
 *
 * Primer poziva:
 * ./filterByTime dirpath
 */
#define _XOPEN_SOURCE 700
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define UNUSED(x) ((void)x)

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

long int total_size = 0;

/* Callback funkcija koju nftw() poziva za svaki pronađeni fajl.
 * Deklaraciju ove funkcije mozete kopirati iz `man nftw`.
 */
int calculate_size(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    /* Parametri koji nam nisu potrebni u ovoj implementaciji */
    UNUSED(fpath);
    UNUSED(typeflag);
    UNUSED(ftwbuf);

    /* Dodajemo veličinu trenutnog fajla */
    total_size += sb->st_size;

    /* Povratna vrednost 0 znači da se obilazak direktorijuma nastavlja */
    /* Vrednost različita od 0 bi prekinula rekurziju */
    return 0;
}

int main(int argc, char **argv)
{
    check_error(argc == 2, "./filterExt path ");

    /* provera da li se radi o direktorijumu */
    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat failed");
    check_error(S_ISDIR(fInfo.st_mode), "not a dir");

    /* obilazak direktorijuma pomocu ugradjene funkcije */
    check_error(nftw(argv[1], calculate_size, 50, 0) != -1, "nftw failed");

    printf("Velicina direktorijuma je %ld\n", total_size);
    exit(EXIT_SUCCESS);
}
