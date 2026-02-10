#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
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
    check_error(argc == 2, "./a.out filepath");

    int childPid = fork();
    check_error(childPid != -1, "fork");

    if (childPid == 0) {
        /* dete izvrsava zadatu komandu */
        check_error(execlp("gcc", "gcc", "-std=c99", "-S", argv[1], "-o", "2.s", NULL) != -1,
                    "execlp");
    }

    int status;
    check_error(wait(&status) != -1, "wait");

    if (WIFEXITED(status)) {
        /* ukoliko je dete exit-ovalo, izdvajamo exit kod */
        int exit_code = WEXITSTATUS(status);

        if (exit_code == 0) {
            FILE *f = fopen("2.s", "r");
            check_error(f != NULL, "fopen");

            char *linija = malloc(1024);
            check_error(linija != NULL, "malloc");

            int brojac = 0;
            while (fgets(linija, 1024, f) != NULL) {
                if (strstr(linija, "mov")) {
                    brojac++;
                }
            }
            printf("%d\n", brojac);

            fclose(f);
            free(linija);
        } else {
            /* ukoliko dete nije uspelo, exit-ujemo sa istim exit kodom kao i dete */
            exit(exit_code);
        }
    } else {
        check_error(0, "dete nasilno prekinulo rad");
    }

    return 0;
}