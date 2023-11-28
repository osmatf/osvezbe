#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#include <limits.h>
#include <strings.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)
    
static const char *os_Usage = "./1";
    
void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);

#define RD_END (0)
#define WR_END (1)
#define MAX_SIZE (256)

int main(int argc, char** argv) {
	
	check_error(argc == 1, os_Usage);
	
	/* za dvosmernu komunikaciju uvek moraju da se koriste dva pajpa */
	int par2cld[2];	/* pipe: smer roditelj -> dete */
	int cld2par[2];	/* pipe: smer dete -> roditelj */
	
	/* kreiramo pajpove */
	check_error(pipe(par2cld) != -1, "pipe creation failed");
	check_error(pipe(cld2par) != -1, "pipe creation failed");
	
	pid_t childPid = fork();
	check_error(childPid != -1, "fork failed");
	
	if (childPid > 0) { /* parent */
		
		/* prvo se zatvaraju deskriptori koji se ne koriste */
		close(par2cld[RD_END]);
		close(cld2par[WR_END]);
		
		/* argumenti za getline */
		char* line = NULL;
		size_t lineLen = 0;
		char retVal[32];
		
		/* u beskonacnoj petlji */
		while (1) {
			
			/* ucitamo liniju */
			check_error(getline(&line, &lineLen, stdin) != -1, "getline failed");
			
			/* ako je korisnik uneo quit */
			if (!strcasecmp(line, "Quit\n")) {
				
				printf("Bye\n");
				/* saljemo SIGKILL signal detetu */
				check_error(kill(childPid, SIGKILL) != -1, "Kill failed");
				/* iskacemo iz petlje */
				break;
			}
			
			check_error(strlen(line) < MAX_SIZE, "line too long");
			
			/* saljemo komandu detetu */
			check_error(write(par2cld[WR_END], line, strlen(line)) != -1, "write failed");
			
			/* sacekamo da child vrati exit kod */
			int exitCode = 0;
			memset(retVal, 0, 32);
			check_error(read(cld2par[RD_END], retVal, 32) != -1, "read failed");
			
			/* procitamo exit kod */
			sscanf(retVal, "%d", &exitCode);
			
			/* ispisemo odgovarajucu poruku */
			if (WIFEXITED(exitCode)) {
			
				if (WEXITSTATUS(exitCode) == EXIT_SUCCESS) {
					printf("Success\n");
				}
				else {
					printf("Failure\n");
				}
			}
			else {
				printf("Failure\n");
			}
		}
		
		/* oslobadjanje alocirane memorije */
		free(line);
		
		/* zatvaraju se i preostali deskriptori */
		close(par2cld[WR_END]);
		close(cld2par[RD_END]);
	}
	else { /* child */
		
		/* prvo se zatvaraju deskriptori koji se ne koriste */
		close(par2cld[WR_END]);
		close(cld2par[RD_END]);
		
		/* lokalni baferi */
		char command[MAX_SIZE];
		char response[32];
		
		/* preusmeravamo streamove ni na sta */
		check_error(freopen("/dev/null", "w", stdout) != NULL, "freopen failed");
		check_error(freopen("/dev/null", "w", stderr) != NULL, "freopen failed");
		
		/* u beskonacnoj petlji */
		while (1) {
			
			/* brisemo prethodni sadrzaj bafera */
			memset(command, 0, 32);
			
			/* cekamo komandu od parenta */
			check_error(read(par2cld[RD_END], command, MAX_SIZE) != -1, "read failed");
			
			/* izvrsavamo komandu */
			int retVal = system(command);
			
			/* upisujemo exit kod komande */
			sprintf(response, "%d", retVal);
			
			/* saljemo exit kod roditelju */
			check_error(write(cld2par[WR_END], response, strlen(response)) != -1, "write failed");
		}
		
		/* zatvaraju se i preostali deskriptori */
		close(par2cld[RD_END]);
		close(cld2par[WR_END]);
	}
	
	/* nakon svakog forka mora da se pozove wait ili waitpid da se ne
	 * sejali zombiji po sistemu
	 */ 
	check_error(wait(NULL) != -1, "wait failed");
	
	exit(EXIT_SUCCESS);
}
