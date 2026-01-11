/*
Program demonstrira kako od file deskriptora pravimo odgovarajuci
stream, tako sto od fd koji se odnosi na jednu stranu pajpa kreiramo FILE *
koji se kasnije moze koristiti za formatiran I/O.
U ovom primeru, roditelj salje detetu formatiranu poruku preko pajpa,
a dete je cita i ispisuje na svoj standardni izlaz.
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

#define PIPE_RD (0)
#define PIPE_WR (1)
#define MAX_MSG_LEN (256)

int main(int argc, char **argv)
{
    check_error(1 == argc, "Usage: ./a.out path/to/reg/file");

    int pipeFds[2];
    check_error(-1 != pipe(pipeFds), "Opening pipe failed");

    pid_t child = fork();
    check_error(-1 != child, "Forking child failed");

    /*
     * Interprocesnu komunikaciju ostvarujemo pomoću standardne biblioteke (stdio).
     *
     * BITNO:
     * Pajp je bajtno-orijentisan kanal. Korišćenje read()/write() za
     * formatirani ulaz/izlaz (stringovi, brojevi, strukture u tekstualnom obliku)
     * nije preporučljivo, jer ne postoji garancija da će jedan read()/write()
     * obuhvatiti “celu poruku”.
     *
     * Poziv read() može da se završi:
     *  - na sredini reči
     *  - na sredini broja
     *  - na granici bafera
     *
     * Zbog toga debagovanje postaje složeno i prepuno graničnih slučajeva.
     * Korišćenjem stdio funkcija (fprintf, fgets, fscanf) dobijamo:
     *  - baferisanje
     *  - rad sa formatiranim podacima
     *  - čitljiviji i sigurniji kod
     */
    if (child) { // Parent
        const char *messagePrivateToParent = "Hello my child how are you doing?";
        const int magicNumber = 524287;

        /*
         * Zatvaramo stranu koju ne koristimo
         */
        close(pipeFds[PIPE_RD]);

        /*
         * Fajl deskriptor za pisanje pretvaramo u stdio stream.
         * fdopen() NE otvara novi fajl deskriptor, već postojeći
         * vezuje za FILE* strukturu.
         */
        FILE *pipeFileStream = fdopen(pipeFds[PIPE_WR], "w");
        check_error(NULL != pipeFileStream,
                    "Creating C stream from file descriptor failed (parent)");

        /*
         * Detetu šaljemo formatiranu poruku pomoću standardne biblioteke.
         * Podaci se upisuju u bafer i biće poslati kroz pajp prilikom
         * pražnjenja bafera ili zatvaranja streama.
         */
        fprintf(pipeFileStream, "%s Magic: %d", messagePrivateToParent, magicNumber);

        /*
         * Zatvaranjem file streama automatski se:
         *  - isprazni bafer (fflush)
         *  - zatvori pridruženi fajl deskriptor pajpa
         */
        fclose(pipeFileStream);
    } else { // Child
        /*
         * Zatvaramo stranu koju ne koristimo
         */
        close(pipeFds[PIPE_WR]);

        char buf[MAX_MSG_LEN];

        /*
         * Fajl deskriptor za čitanje pretvaramo u stdio stream.
         * Time dobijamo mogućnost korišćenja funkcija poput fgets().
         */
        FILE *pipeFileStream = fdopen(pipeFds[PIPE_RD], "r");
        check_error(NULL != pipeFileStream,
                    "Creating C stream from file descriptor failed (child)");

        /*
         * Čitamo poruku poslatu od parent procesa.
         */
        check_error(NULL != fgets(buf, MAX_MSG_LEN, pipeFileStream), "Getting message failed");
        /*
         * Štampamo primljenu poruku na standardni izlaz (terminal).
         */
        printf("[child] Got: '%s'\n", buf);

        /*
         * Zatvaranjem streama:
         *  - oslobađamo stdio resurse
         *  - zatvaramo fajl deskriptor pajpa
         */
        fclose(pipeFileStream);

        /* zavrsavamo child proces */
        exit(EXIT_SUCCESS);
    }

    /*
     * Parent proces čeka završetak child procesa.
     * Na ovaj način se izbegava nastanak zombi procesa.
     */
    check_error(-1 != wait(NULL), "Waiting for child failed");

    return 0;
}
