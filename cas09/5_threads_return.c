/*
Napisati program koji sa standardnog ulaza učitava broj n a potom i n celih brojeva.
Pokrenuti n niti, pri čemu i-ta nit nalazi zbir cifara i-tog broja
u nizu i ažurira globalnu sumu cifara svih brojeva u nizu,
pri čemu se zbir cifara i-tog broja vraća kroz povratnu vrednost niti.
Na kraju, na standardni izlaz ispisati izračunatu sumu.
Napomena: globalna suma cifara svih brojeva ne sme biti izračunata u main-u.
Za sinhronizaciju koristiti muteks.
*/

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define checkError(exp, userMsg)                                                                   \
    do {                                                                                           \
        if (!(exp)) {                                                                              \
            fprintf(stderr, "Fatal error: %s\n", userMsg);                                         \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

#define checkErrorPthread(exp, userMsg)                                                            \
    do {                                                                                           \
        int _err = exp;                                                                            \
        if ((_err) > 0) {                                                                          \
            errno = _err;                                                                          \
            fprintf(stderr, "Fatal error: %s\n", userMsg);                                         \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

typedef struct {
    int x;
} inARg_t;

typedef struct {
    int x;
} outArg_t;

int globalSum = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *threadFcn(void *arg)
{
    int x = abs(((inARg_t *)arg)->x);
    int sum = 0;

    while (x > 0) {
        sum += x % 10;
        x /= 10;
    }

    checkErrorPthread(pthread_mutex_lock(&mutex), "lock");
    globalSum += sum;
    checkErrorPthread(pthread_mutex_unlock(&mutex), "unlock");

    outArg_t *out = malloc(sizeof(outArg_t));
    checkError(out != NULL, "malloc");

    out->x = sum;

    return out;
}

int main(int argc, char **argv)
{

    checkError(argc == 1, "args");
    int n = 0;
    scanf("%d", &n);

    inARg_t *niz = malloc(n * sizeof(inARg_t));
    checkError(niz != NULL, "malloc");

    for (int i = 0; i < n; i++) {
        scanf("%d", &(niz[i].x));
    }

    pthread_t *tids = malloc(n * sizeof(pthread_t));
    checkError(tids != NULL, "malloc");

    for (int i = 0; i < n; i++) {

        checkErrorPthread(pthread_create(&tids[i], NULL, threadFcn, &niz[i]), "create");
    }

    for (int i = 0; i < n; i++) {

        outArg_t *retVal = NULL;
        checkErrorPthread(pthread_join(tids[i], (void **)&retVal), "join");

        free(retVal);
    }

    printf("%d\n", globalSum);

    free(niz);

    exit(EXIT_SUCCESS);
}