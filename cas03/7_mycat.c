/*
Napisati program koji ispisuje sadrzaj prosledjenog fajla
na standardni izlaz koriscenjem sistemskih poziva read i write.

Poziv programa:
./os_cat filename
*/

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <errno.h>
#include <stdbool.h>
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

#define BUFFER_SIZE 4096

int main(int argc, char **argv)
{
    check_error(2 == argc, "./a.out filepath");

    char *buffer = malloc(BUFFER_SIZE);
    check_error(buffer != NULL, "malloc");

    int fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open");

    int procitano;
    /* sve dok mozemo da ucitamo nesto iz fajla */
    while ((procitano = read(fd, buffer, BUFFER_SIZE)) > 0) {
        /* ono sto smo procitali stampamo na stdout
         *
         * BITNO: kada se koriste stdout, stdin i stderr u kombinaciji sa read i write, uvek morate
         * da koristite sistemski definisane fajl deskriptore:
         *
         * stdout -> STDOUT_FILENO
         * stderr -> STDERR_FILENO
         * stdin  -> STDIN_FILENO
         *
         * Nikada ne smete da koristite brojeve umesto sistemski definisanih konstanti, jer to
         * zavisi od implementacije.
         *
         * BITNO 2:
         * uvek mozete da upisete u fajl najvise onoliko bajtova koliko ste procitali. Dakle,
         * argument funkcije write moze da bude samo procitano, nikako BUFFER_SIZE
         */
        check_error(write(STDOUT_FILENO, buffer, procitano) == procitano, "write");
    }

    /* BITNO: sve sto se alocira, mora i da se oslobodi */
    free(buffer);

    return 0;
}
