/*
Poziv programa;
./a.out path ext

Program dodaje ekstenziju 'ext' na ime fajla koji se nalazi
na putanji 'path' (slicno kao rename funkcija).
*/

#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <time.h>

#include <string.h>

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

#define MAX_SIZE (256)

int main(int argc, char **argv)
{
    check_error(argc == 3, "./a.out filepath ext");

    /* Dužina putanje do fajla */
    int pathLen = strlen(argv[1]);

    /* Pronalazak tačke koja označava početak ekstenzije */
    char *eks = strrchr(argv[1], '.');
    check_error(eks != NULL, "fajl nema ekstenziju");

    /* Dužina stare ekstenzije */
    int eksLen = strlen(eks);

    /* Alokacija memorije za novo ime fajla (bez stare + nova ekstenzija) */
    char *newName = malloc(strlen(argv[1]) + strlen(argv[2]) + 1);
    check_error(newName != NULL, "...");

    /* Kopiranje dela imena bez stare ekstenzije */
    strncpy(newName, argv[1], pathLen - eksLen);

    /* Dodavanje nove ekstenzije */
    strcat(newName, argv[2]);

    int srcFd = open(argv[1], O_RDONLY);
    check_error(srcFd != -1, "open");

    /* Kreiranje (ili brisanje sadržaja) odredišnog fajla za pisanje */
    int destFd = open(newName, O_WRONLY | O_TRUNC | O_CREAT);
    check_error(destFd != -1, "open");

    /* Dohvatanje informacija o izvornom fajlu */
    struct stat fInfo;
    check_error(fstat(srcFd, &fInfo) != -1, "fstat");

    char buffer[MAX_SIZE];
    int procitano = 0;

    /* Kopiranje sadržaja fajla blok po blok */
    while ((procitano = read(srcFd, buffer, MAX_SIZE)) > 0) {
        check_error(write(destFd, buffer, procitano) != -1, "write");
    }

    check_error(procitano != -1, "read");

    close(srcFd);
    close(destFd);

    /* Brisanje originalnog fajla */
    check_error(unlink(argv[1]) != -1, "unlink");

    /* Postavljanje prava pristupa novom fajlu kao kod originalnog */
    check_error(chmod(newName, fInfo.st_mode) != -1, "chmod");

    free(newName);

    exit(EXIT_SUCCESS);
}
