/*
Napisati program koji ispisuje vreme pristupa, modifikacije
i poslednje promene statusa fajla. Dodatno, ispisati i trenutno vreme
na sistemu.

Poziv programa:
./fileinfo filePath
*/
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

int main(int argc, char *argv[])
{
    check_error(argc == 2, "argc");
    /** Struktura stat sadrzi:
     *
     * atime (st_atime) – vreme poslednjeg pristupa fajlu
     * (npr. čitanje fajla).
     *
     * mtime (st_mtime) – vreme poslednje izmene sadržaja fajla.
     *
     * ctime (st_ctime) – vreme poslednje promene metapodataka fajla
     * (npr. prava pristupa, vlasnik, ili sadržaj).
     *
     */
    struct stat fileinfo;
    check_error(stat(argv[1], &fileinfo) != -1, "stat");

    /*
     * Vremena st_atime, st_mtime i st_ctime u strukturi stat
     * čuvaju se kao broj sekundi proteklih od 1. 1. 1970.
     * (tzv. Unix epoch).
     *
     * Funkcija ctime() se koristi da bi se ova numerička vrednost
     * (tipa time_t)  pretvorila u čitljiv tekstualni
     * zapis datuma i vremena.
     *
     * Time se izbegava ručno računanje godina, meseci, dana,
     * sati, minuta i sekundi, kao i komplikacije sa prestupnim
     * godinama i vremenskim zonama.
     */
    printf("Access time (atime): %s", ctime(&fileinfo.st_atime));
    printf("Modification time (mtime): %s", ctime(&fileinfo.st_mtime));
    printf("Change time (ctime): %s", ctime(&fileinfo.st_ctime));
    /*
     * time() vraca sistemsko vreme u trenutku izvršavanja programa.
     */
    time_t now = time(NULL);
    check_error(now != (time_t)-1, "time");

    printf("Current time: %s", ctime(&now));

    return 0;
}