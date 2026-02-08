#define _GNU_SOURCE
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <signal.h>

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)
int state = 0;
int shouldTerminate = 0;

void sigHandler(int signum)
{

    switch (signum) {
    case SIGUSR1:
        state = 1;
        break;
    case SIGUSR2:
        state = 2;
        break;
    case SIGINT:
        state = 3;
        break;
    case SIGQUIT:
        shouldTerminate = true;
        break;
    }
}

unsigned obrnut(unsigned x)
{

    unsigned result = 0;

    while (x) {

        result = result * 10 + x % 10;
        x /= 10;
    }

    return result;
}

int main(int argc, char **argv)
{
    check_error(argc == 1, "./a.out");
    unsigned x, y;

    check_error(signal(SIGUSR1, sigHandler) != SIG_ERR, "signal error");
    check_error(signal(SIGUSR2, sigHandler) != SIG_ERR, "signal error");
    check_error(signal(SIGINT, sigHandler) != SIG_ERR, "signal error");
    check_error(signal(SIGQUIT, sigHandler) != SIG_ERR, "signal error");

    do {

        scanf("%u", &x);

        pause();

        switch (state) {

        case 1:
            printf("%u\n", x * x);
            break;
        case 2:
            printf("%u\n", x * x * x);
            break;
        case 3:
            printf("%u\n", obrnut(x));
            break;
        }

        state = 0;

    } while (!shouldTerminate);

    exit(EXIT_SUCCESS);
}
