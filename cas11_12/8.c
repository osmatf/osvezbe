#define _POSIX_C_SOURCE 200112L
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    check_error(argc == 1, "./a.out");

    /* izdvajamo vrednost trazene promenljive okruzenja */
    char *path = getenv("PATH");
    check_error(path != NULL, "getenv");

    /* alociramo memoriju za filtriranu promenljivu,
     * a posto je filtrirana mora imati manje karaktera nego polazna
     */
    char *path_filtered = malloc(strlen(path) + 1);
    check_error(path_filtered != NULL, "malloc");
    /* s obzirom da uvek nadovezujemo na trenutni string, moramo obezbediti da je na pocetku prazan
     * string */
    path_filtered[0] = '\0';

    /* indikator koji nam kaze da li je u pitanju prvi direktorijum u filtriranoj promenljivoj*/
    bool first = true;

    /* izdvajamo prvi token */
    char *next_token = strtok(path, ":");
    do {
        struct stat fileinfo;
        /* gledamo errno da program ne bi pukao u slucaju nepostojanja direktorijuma */
        errno = 0;
        stat(next_token, &fileinfo);
        if (errno != 0) {
            if (errno == ENOENT) {
                continue;
            } else {
                check_error(0, "stat");
            }
        }

        if (fileinfo.st_mode & S_IWGRP) {
            /* kada dodajemo prvu putanju u string, ne dodajemo karakter ':' */
            if (!first) {
                strcat(path_filtered, ":");
            }
            first = false;
            strcat(path_filtered, next_token);
        }
        /* zavrsavamo iteraciju izdvajajuci sledeci token */
    } while ((next_token = strtok(NULL, ":")) != NULL);

    /* postavljamo novoformiranu promenljivu okruzenja */
    check_error(setenv("PATH_FILTERED", path_filtered, 1) != -1, "setenv");

    free(path_filtered);
    return 0;
}