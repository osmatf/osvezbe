#define _DEFAULT_SOURCE
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

bool sigusr1;
bool sigusr2;
bool sigquit;

void obradi_signal(int signum)
{
    if (signum == SIGUSR1) {
        sigusr1 = true;
    } else if (signum == SIGUSR2) {
        sigusr2 = true;
    } else {
        sigquit = true;
    }

    return;
}

int main(int argc, char **argv)
{
    check_error(argc == 2, "./a.out seconds");

    char *putanja = getenv("PUTANJA");
    check_error(putanja != NULL, "getenv");

    check_error(signal(SIGUSR1, obradi_signal) != SIG_ERR, "signal");
    check_error(signal(SIGUSR2, obradi_signal) != SIG_ERR, "signal");
    check_error(signal(SIGQUIT, obradi_signal) != SIG_ERR, "signal");

    do {
        pause();
        if (sigquit) {
            break;
        }

        if (sigusr1) {
            struct stat fileinfo;
            check_error(lstat(putanja, &fileinfo) != -1, "lstat");

            int godina_pristupa, godina_modifikacije;

            struct tm *bdt1 = localtime(&fileinfo.st_atime);
            godina_pristupa = bdt1->tm_year + 1900;
            struct tm *bdt2 = localtime(&fileinfo.st_mtime);
            godina_modifikacije = bdt2->tm_year + 1900;

            printf("%d %d\n", godina_pristupa, godina_modifikacije);
            sigusr1 = false;
        }
        if (sigusr2) {

            struct utimbuf vremena;
            vremena.actime = atoi(argv[1]);
            vremena.modtime = atoi(argv[1]);

            check_error(utime(putanja, &vremena) != -1, "utime");
            printf("Uspeno postavljena vremena fajlu %s\n", putanja);

            sigusr2 = false;
        }
    } while (1);

    printf("Gotova obrada signala\n");

    return 0;
}