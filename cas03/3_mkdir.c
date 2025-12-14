/*
Napisati program koji kreira direktorijum sa imenom
koje se zadaje kao argument komandne linije, pri cemu
za prava pristupa tom direktorijimu koristiti 0755.

Poziv programa:
./a.out dir_name
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

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
    check_error(argc == 3, "./a.out dirname privileges");

    /*
     * Argumenti komandne linije su niske (char *), pa je potrebno
     * izvršiti njihovu konverziju u numeričku vrednost tipa mode_t.
     * Funkcija strtol() se koristi za tu konverziju.
     *
     * Treći argument funkcije strtol() je 8, što znači da se ulazna
     * niska tumači kao oktalni broj.
     *
     * Privilegije fajlova i direktorijuma zapisuju se kao tri trojke
     * bitova:
     *
     *   u   g   o
     *  rwx rwx rwx
     *
     * Svaka trojka ima tačno tri bita:
     *   - prvi bit:  pravo čitanja (r)
     *   - drugi bit: pravo pisanja (w)
     *   - treći bit: pravo izvršavanja (x)
     *
     * Oktalna cifra direktno odgovara toj trojci bitova:
     *
     *   0 -> ---
     *   1 -> --x
     *   2 -> -w-
     *   3 -> -wx
     *   4 -> r--
     *   5 -> r-x
     *   6 -> rw-
     *   7 -> rwx
     *
     * Zapis 0755 znači:
     *   user:  rwx
     *   group: r-x
     *   other: r-x
     *
     * Zbog toga se pri unosu privilegija koristi oktalni zapis,
     * a strtol() omogućava da se taj zapis pravilno protumači.
     */
    mode_t mode = strtol(argv[2], NULL, 8);

    check_error(mkdir(argv[1], mode) != -1, "mkdir");

    return 0;
}
