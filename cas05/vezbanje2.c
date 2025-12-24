/*
Rekurzivnim obilaskom, u direktorijumu cija se putanja zadaje kao prvi argument komandne linije,
pronaci fajl sa nazivom koji se zadaje kao drugi argument komandne linije.
Ispisati apsolutnu putanju do trazenog fajla.

Napomena: cim program pronadje fajl, prekinuti rekurzivni obilazak direktorijuma.

Poziv programa:
./a.out
*/
#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <ftw.h>

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

char *filename = NULL;

int pronadji_fajl(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    UNUSED(sb);
    UNUSED(typeflag);
    /*
     * Poredi se samo ime fajla (bez putanje) sa traženim imenom.
     * ftwbuf->base predstavlja indeks u nizu fpath odakle počinje ime fajla.
     */
    if (strcmp(fpath + ftwbuf->base, filename) == 0) {
        /* realpath() vraća apsolutnu kanonizovanu putanju do fajla */
        char *abs_path = realpath(fpath, NULL);
        check_error(abs_path != NULL, "realpath");

        printf("%s\n", abs_path);
        free(abs_path);
        /*
         * Povratna vrednost različita od 0 signalizira nftw() funkciji
         * da prekine dalje rekurzivno obilazak direktorijuma.
         *
         * Ovo omogućava da se program zaustavi čim se fajl prvi put pronađe,
         * umesto da nastavi nepotrebno pretraživanje ostatka stabla.
         */
        return 1;
    }

    /* Povratna vrednost 0 znači da se obilazak direktorijuma nastavlja */
    return 0;
}

int main(int argc, char **argv)
{
    check_error(argc == 3, "./a.out dirpath filename");

    /* Provera da li prvi argument predstavlja direktorijum */
    struct stat fileinfo;
    check_error(lstat(argv[1], &fileinfo) != -1, "lstat");
    check_error(S_ISDIR(fileinfo.st_mode), "nije direktorijum");

    /* Čuvamo ime fajla globalno, jer nftw callback funkcija `pronadji_fajl` nema dodatne argumente
     */
    filename = argv[2];

    check_error(nftw(argv[1], pronadji_fajl, 50, 0) != -1, "nftw");

    exit(EXIT_SUCCESS);
}
