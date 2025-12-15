/*
Napisati program koji ispisuje velicinu fajla koriscenjem
sistemskog poziva lseek.

Napomena:
- ovo je pogresan nacin za izracunavanje velicine fajla.
Da bi program radio, neophodno je pravo citanja nad
zadatim fajlom, sto ne bi trebalo da bude slucaj,
jer zapravo ne citamo sadrzaj fajla, već meta informacije
o njemu. Za velicinu fajla koristimo sistemski poziv stat.

Poziv programa:
./filesize filename

Domaci:
Umesto sa regularnim fajlom, probati sa standardnim izlazom (STDOUT_FILENO).
lseek ce puci. Zasto?
*/
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
    check_error(2 == argc, "./a.out filepath");

    /*
     * Otvaramo fajl u modu za citanje
     */
    int fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open");

    /*
     * Funkcija lseek pomera offset fajla. Pomeranjem offseta na kraj,
     * kao posledicu dobijamo velicinu fajla
     *
     * TLPI za detalje
     */
    off_t fileSize = lseek(fd, 0, SEEK_END);
    check_error(fileSize >= 0, "Lseek failed");

    close(fd);

    printf("%jd\n", (intmax_t)fileSize);

    return 0;
}
