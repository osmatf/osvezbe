/*
Program demonstrira kako od file deskriptora pravimo odgovarajuci
stream, tako sto od fd koji se odnosi na jednu stranu pajpa kreiramo FILE *
koji se kasnije moze koristiti za formatiran I/O.
U ovom primeru, roditelj salje detetu formatiranu poruku preko pajpa,
a dete je cita i ispisuje na svoj standardni izlaz.
*/

#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/wait.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

static const char *os_Usage = "Usage: ./stdio_syscalls_connection path/to/reg/file";

#define PIPE_RD (0)
#define PIPE_WR (1)
#define MAX_MSG_LEN (256)

int main(int argc, char **argv) {
    check_error(1 == argc, os_Usage);

    printf("\nSYSCALLS to C STDIO demnostration:\n===================================\n");
    
    /*
     * Kreiramo pajp
     */
    int pipeFds[2];
    check_error(-1 != pipe(pipeFds), "Opening pipe failed");
	/* forkujemo procese */
    pid_t child = fork();
    check_error(-1 != child, "Forking child failed");
    
    /*
     * Interprocesnu komunikaciju ostvarajuemo pomocu standardne biblioteke
	 * BITNO:
	 * read/write nema smisla koristiti kada se radi o formatiranom ulazu/izlazu jer nikada
	 * nemate garanciju gde ce se read/write prekinuti, tj. da li ce vam poziv puci na pola reci ili pola broja
	 * pa debagovanje postaje nocna mora zbog previse granicnih slucajeva.
	 * U tom slucaju mnogo je lakse i prirodnije koristiti standardnu biblioteku.
     */
    if (child) { // Parent
        const char *messagePrivateToParent = "Hello my child how are you doing?";
        const int magicNumber = 524287;
        
        /*
		 * Zatvaramo stranu koju ne koristimo
		 */
        close(pipeFds[PIPE_RD]);
		/* fajl deskriptoru pridruzujemo strim */
        FILE *pipeFileStream = fdopen(pipeFds[PIPE_WR], "w");
        check_error(NULL != pipeFileStream, "Creating C stream from file descriptor failed (parent)");
		/* detetu saljemo formatirannu poruku preko standardne biblioteke */
        fprintf(pipeFileStream, "%s Magic: %d", messagePrivateToParent, magicNumber);
		/* zatvaranjems trima, automatski se zatvara i fajl deskriptor */
        fclose(pipeFileStream);
    } else { // Child
	
		/*
		 * Zatvaramo stranu koju ne koristimo
		 */
        close(pipeFds[PIPE_WR]);
        
        char buf[MAX_MSG_LEN];
		/* fajl deskriptoru pridruzujemo strim */
        FILE *pipeFileStream = fdopen(pipeFds[PIPE_RD], "r");
        check_error(NULL != pipeFileStream, "Creating C stream from file descriptor failed (child)");
		/* citamo poruku od parenta pomocu standardne biblioteke */
        check_error(NULL != fgets(buf, MAX_MSG_LEN, pipeFileStream), "Getting message failed");
		/* stampamo primljenu poruku na stdout */
        printf("Got: '%s'\n", buf);
        
        /* zatvaranjem strima, automatski se zatvara i fajl deskriptor */
        fclose(pipeFileStream);
		
		/* zavrsavamo child proces */
        exit(EXIT_SUCCESS);
    }
	
    /* cekamo da se child zavrsi i izbegavamo sejanje zombija po sistemu */
    check_error(-1 != wait(NULL), "Waiting for child failed");
	
    return 0;
}
