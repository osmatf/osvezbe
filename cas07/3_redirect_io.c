/*
Program demonstrira preusmeravanje ulaza/izlaza. Kreira se dete proces
koje sa roditeljom komunicira preko pajpa. Pomocu sistemskog
poziva dup2, preusmerava se standardni izlaz deteta, tako da sve
sto bi islo na njegov standardni izlaz ode u pajp.
U ovom primeru dete stampa poruku na svoj standardni izlaz, koja zbog
prethodnog preusmeravanja zavrsava kod roditelja.
*/

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

int main()
{
    /*
     * Kreiramo pajp
     */
    int pipeFds[2];
    check_error(-1 != pipe(pipeFds), "Opening pipe failed");

    /*
     * Forkujemo procese
     */
    pid_t child = fork();
    check_error(-1 != child, "Fork failed");
    if (child) { // parent
                 /* zatvaramo stranu pajpa koja se ne koristi */
        close(pipeFds[PIPE_WR]);

        int bytesRead;
        char buf[MAX_MSG_LEN];
        /* u parent grani cekamo poruku od child-a */
        check_error(-1 != (bytesRead = read(pipeFds[PIPE_RD], buf, MAX_MSG_LEN)),
                    "Reading message from child in parent failed");
        /* primljenu poruku stampamo na stdout */
        check_error(-1 != write(STDOUT_FILENO, buf, bytesRead),
                    "Writing message to stdout failed [parent]");
        /* zatvaramo preostalu stranu pajpa */
        close(pipeFds[PIPE_RD]);
    } else {
        /* zatvaramo stranu pajpa koja se ne koristi */
        close(pipeFds[PIPE_RD]);

        /*
         * Preusmeravamo stranu pajpa za citanje na stdout
         *
         * BITNO:
         * dup2 je jedina funkcija koju smo radili da radi "naopako", tj. prvi argument je stara
         * vrednost fajl deskriptora, a drugi argument je nova vrednost fajl deksriptora
         *
         * dup2(old, new)
         *
         * U ovom primeru zelimo da postignemo da strana pajpa za pisanje bude stdout, tj. hocemo da
         * izvrstimo redirekciju. Kod koji to postize je sledeci:
         *
         * dup2(pipeFds[PIPE_WR], STDOUT_FILENO);
         */
        check_error(-1 != dup2(pipeFds[PIPE_WR], STDOUT_FILENO),
                    "Redirecting child's stdout failed");

        /*
         * Preusmeravanjem smo postigli da poruku parentu mozemo da posaljemo koristeci printf. Ovo
         * je moguce zato sto printf izlaz stampa na stdout, na koji smo mi pomocu dup2 preusmerili
         * fajl desktiptor pajpa. Na taj nacin je stdout ustvari kraj pajpa za pisanje.
         */
        printf("This message magically appears in parent!\n");
        close(pipeFds[PIPE_WR]);
        exit(EXIT_SUCCESS);
    }

    /* cekamo da se child zavrsi i izbegavamo sejanje zombija po sistemu */
    check_error(-1 != wait(NULL), "Waiting for child failed");

    return 0;
}
