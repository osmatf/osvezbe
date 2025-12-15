/*
Napisati program koji kopira fajl sa jedne lokacije na drugu.

Poziv programa:
./a.out destPath srcPath
*/
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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
    check_error(3 == argc, "./a.out destPath srcPath");

    int fdSrc = open(argv[2], O_RDONLY);
    check_error(fdSrc != -1, "open");

    /* Dohvatanje informacija o izvornom fajlu (npr. privilegija) */
    struct stat fileinfo;
    check_error(stat(argv[2], &fileinfo) != -1, "stat");

    /**
     * Otvaranje (ili kreiranje) destinacionog fajla.
     * Privilegije ce kasnije biti podesene pomocu chmod.
     *
     */
    int fdDest = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    check_error(fdDest != -1, "open");

    char *buffer = malloc(BUFFER_SIZE);
    check_error(buffer != NULL, "malloc");

    int procitano;
    errno = 0;
    while ((procitano = read(fdSrc, buffer, BUFFER_SIZE)) > 0) {
        check_error(write(fdDest, buffer, procitano) == procitano, "write");
        errno = 0;
    }
    check_error(procitano == 0, "read");

    /* Postavljanje istih privilegija kao kod izvornog fajla */
    check_error(chmod(argv[1], fileinfo.st_mode) != -1, "chmod");

    close(fdSrc);
    close(fdDest);
    free(buffer);
    return 0;
}
