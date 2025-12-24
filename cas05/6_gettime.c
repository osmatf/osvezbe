/* Program demonstrira upotrebu funkcije gettimeofday koja vraca vreme
 * tacnosti do na mikrosekundu.
 Napomena: nije neophodno za ispit, ali može biti korisno

 */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <time.h>

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
    /* funkcija time vraca broj sekundi od epohe */
    time_t now = time(NULL);

    /* funkcija gettimeofday() omogucava preciznije merenje vremena, tj.
     * format vremena je sada u sekundama i mikrosekundama.
     *
     * Vreme u ovom formatu se upisuje u strukturu tipa timeval koja ima
     * dva polja:
     * tv_usec -> mikrosekunde
     * tv_sec -> sekunde
     */
    struct timeval preciseTime;
    check_error(gettimeofday(&preciseTime, NULL) != -1, "lklk");

    /* provera koliko time gresi u odnosu na gettimeofday */
    if (now == preciseTime.tv_sec) {
        printf("time() == gettimeofday()\n");
    } else {
        printf("time() != gettimeofday()\n");
    }
    printf("Time diff: %d\n", (int)preciseTime.tv_usec);

    exit(EXIT_SUCCESS);
}
