/* Program rekurzivno izdvaja sve fajlove sa zadatom
 * ekstenzijom u direktorijumu na putanji koju prosledi korisnik
 *
 * Primer poziva:
 * ./a.out dirpath ext
 */
#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <ftw.h>

#include <string.h>

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

char *ext = NULL;

int filterByExt(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    /* ako se radi o regularnom fajlu */
    if (typeflag == FTW_F) {

        /* ftwbuf->base predstavlja indeks u nizu `fpath` od kog počinje ime fajla
         * (bez putanje do direktorijuma). Na taj način pretragu karaktera '.'
         * ograničavamo samo na ime fajla, a ne na celu putanju, kako tačke iz
         * direktorijuma (npr. ./dir.v1/file) ne bi bile pogrešno protumačene kao
         * ekstenzija fajla.
         */
        char *fExt = strrchr(fpath + ftwbuf->base, '.');
        /* obavezna provera da li je povratna vrednost NULL pre poredjenja
         * seg fault u suprotnom
         */
        if (fExt != NULL && !strcmp(fExt, ext)) {
            /* ispisati ime fajla */
            printf("%-80s\n", fpath);
        }
    }

    /* povratna vrednost funkcije razlicita od 0 signalizira gresku i prekida rekurziju */
    return 0;
}

int main(int argc, char **argv)
{
    check_error(argc == 3, "./filterExt path ext");

    /* pamtimo pokazivac na ekstenziju */
    ext = argv[2];

    /* provera da li se radi o direktorijumu */
    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat failed");
    check_error(S_ISDIR(fInfo.st_mode), "not a dir");

    /* obilazak direktorijuma pomocu ugradjene funkcije */
    check_error(nftw(argv[1], filterByExt, 50, 0) != -1, "nftw failed");

    /* zavrsavamo program sa odgovarajucim statusom */
    exit(EXIT_SUCCESS);
}
