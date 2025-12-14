/*
Ispisati velicinu fajla na standardni izlaz. Putanja do fajla
se prosledjuje kao argument komandne linije.

Poziv programa:
./fileinfo filePath
*/
#include <sys/stat.h>
#include <sys/types.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

    /* informacije o fajlu se dobijaju upotrebom funkcije stat
     * man 2 stat
     *
     * TLPI za detalje
     */
    struct stat fileinfo;
    check_error(stat(argv[1], &fileinfo) != -1, "stat");

    /*
     * Zašto koristiti stat() za dobijanje veličine fajla,
     * umesto da se veličina računa čitanjem fajla?
     *  - stat() vraća metapodatke koje kernel već ima.
     *  - Nema čitanja sadržaja fajla, vremenska složenost je O(1).
     *  - Ne koristi se dodatni bafer za citanje podataka.
     *  - Ne opterećuje se disk I/O operacijama.
     *  - Čitanje fajla može biti zabranjeno, ali stat() i dalje radi.
     *  - Ne postoji rizik da se fajl delimično pročita ili promeni tokom čitanja.
     */
    printf("%jd\n", (intmax_t)fileinfo.st_size);

    exit(EXIT_SUCCESS);
}
