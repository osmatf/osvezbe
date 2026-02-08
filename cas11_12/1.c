#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

#define osCheckPthread(pthreadErr, userMsg)                                                        \
    do {                                                                                           \
        int __pthreadErr = (pthreadErr);                                                           \
        if (__pthreadErr > 0) {                                                                    \
            errno = __pthreadErr;                                                                  \
            check_error(false, userMsg);                                                           \
        }                                                                                          \
    } while (0)

/* tip koji definise ulazni argument za funkciju koja implementira nit */
typedef struct {
    char *fileName; /* ima fajla koji nit treba da obradi */
    int index;      /* lokalni index niti */
} osThreadFunctionArgumentType;

/* tip koji definise povratnu vrednost funkcije koja implementira nit */
typedef struct {
    int score; /* skor fajla */
    int index; /* index niti koja je racunala skor */
} osThreadFunctionReturnType;

#define MAX_SIZE (8192)

pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;
int globalScore; /* globalna promenljiva u kojoj pamtimo globalni skor */

void *osThreadFunction(void *arg)
{
    /* kastujemo ulazni argument u odgovarajuci tip */
    osThreadFunctionArgumentType *args = (osThreadFunctionArgumentType *)arg;
    /* alociramo memoriju za povratnu vrednost
     * BITNO: povratna vrednost je alocirana na hipu, sto znaci da nakon upotrebe
     * povrtne vrednosti moramo eksplicitno da je oslobodimo, jer ce nam u suprotnom
     * cureti memorija
     */
    osThreadFunctionReturnType *retVal = malloc(sizeof(osThreadFunctionReturnType));
    /* provera uspesnosti alokacije */
    check_error(retVal != NULL, "allocation failed");

    /* incijalizujemo povratnu vrednost */
    retVal->index = args->index;
    retVal->score = 0;

    /* lokalni brojaci */
    int numPlus = 0;
    int numMinus = 0;

    /* otvaramo fajl */
    int fd = open(args->fileName, O_RDONLY);
    check_error(fd != -1, "open failed");

    /* alociramo bafer za ucitavanje */
    char buffer[MAX_SIZE];
    int readBytes = 0;

    /* u petlji ucitavamo sve dok mozemo */
    while ((readBytes = read(fd, buffer, MAX_SIZE)) > 0) {

        int i = 0;
        /* brojimo karektere koji nas interesuju */
        for (i = 0; i < readBytes; i++) {

            switch (buffer[i]) {

            case '+':
                numPlus++;
                break;
            case '-':
                numMinus++;
                break;
            default:

                break;
            }
        }
    }

    /* racunamo lokalni skor za fajl */
    retVal->score = numPlus - numMinus;

    /* provera greski za read funkciju */
    check_error(readBytes != -1, "read failed");
    /* zatvaramo fajl */
    close(fd);

    osCheckPthread(pthread_mutex_lock(&global_lock), "lock");
    globalScore += retVal->score;
    osCheckPthread(pthread_mutex_unlock(&global_lock), "unlock");

    /* vracamo vrednost iz niti */
    pthread_exit((void *)retVal);
}

int main(int argc, char **argv)
{
    check_error(argc >= 2, "./a.out file0 file1 ... fileN");

    int numOfThreads = argc - 1;
    /* alociramo prostor za ID-ove niti */
    pthread_t *tids = malloc(numOfThreads * sizeof(pthread_t));
    check_error(tids != NULL, "tis Allocation failed");

    /* alociramo prostor za ulazne argumente niti */
    osThreadFunctionArgumentType *args =
        malloc(numOfThreads * sizeof(osThreadFunctionArgumentType));
    check_error(args != NULL, "args allocation failed");

    /* pokazivac na povratnu vrednost niti */
    osThreadFunctionReturnType *retVal = NULL;

    /* u petlji startujemo niti */
    int i = 0;
    for (i = 0; i < numOfThreads; i++) {
        args[i].index = i;
        args[i].fileName = argv[i + 1];
        osCheckPthread(pthread_create(&tids[i], NULL, osThreadFunction, (void *)&args[i]),
                       "thread creation failed");
    }

    /* u petlji join-ujemo niti */
    for (i = 0; i < numOfThreads; i++) {
        osCheckPthread(pthread_join(tids[i], (void **)&retVal), "join failed");

        printf("(%d,%d)\n", retVal->index, retVal->score);
        /* BITNO:
         * povratnu vrednost iz niti moramo eksplicitno da oslobodimo da nam ne bi
         * curela memorija
         */
        free(retVal);
    }

    /* stampamo vrednost globalnog skora */
    printf("%d\n", globalScore);

    /* oslobadjamo alociranu memoriju */
    free(tids);
    free(args);

    exit(EXIT_SUCCESS);
}
