/*
Program demonstrira kako od FILE * mozemo dobiti odgovarajuci
file descriptor i nadalje raditi operacije nad tim fd.
*/

#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

static const char *os_Usage = "Usage: ./stdio_syscalls_connection path/to/reg/file";

#define PIPE_RD (0)
#define PIPE_WR (1)
#define MAX_MSG_LEN (256)

int main(int argc, char **argv)
{
    check_error(2 == argc, os_Usage);

    FILE *fileDem = fopen(argv[1], "r");
    check_error(NULL != fileDem, "Opening file failed");

    /*
     * Iz postojećeg stdio streama dohvatamo pridruženi fajl deskriptor.
     * Funkcija fileno() vraća fajl deskriptor koji stdio interno koristi
     * za realizaciju I/O operacija nad ovim streamom.
     */
    int fileDemFd = fileno(fileDem);
    check_error(-1 != fileDemFd, "Getting file descriptor from FILE* failed");

    /*
     * Upotrebom fstat() nad dobijenim fajl deskriptorom čitamo informacije
     * o fajlu (veličina, prava pristupa, tip fajla, itd.).
     *
     * fstat() radi nad fajl deskriptorom i ne zahteva ime fajla,
     * što je korisno kada je fajl već otvoren.
     */
    struct stat finfo;
    check_error(-1 != fstat(fileDemFd, &finfo), "Getting info through file descriptor failed");

    /*
     * Stampamo velicinu fajla
     */
    printf("File size ('%s'): %jd\n", argv[1], (intmax_t)finfo.st_size);

    /*
     * Zatvaranjem fajl streama pomoću fclose() automatski se zatvara
     * i fajl deskriptor koji je tom streamu pridružen.
     *
     * BITNO:
     * Ne sme se direktno zatvarati fajl deskriptor (close(fd))
     * koji pripada otvorenom streamu, jer to dovodi do
     * nedefinisanog ponašanja programa.
     */
    fclose(fileDem);

    return 0;
}
