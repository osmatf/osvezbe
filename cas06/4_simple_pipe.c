/*
Program implementira nacin za komunikaciju izmedju roditelja i deteta.
Roditelj ucitava liniju sa standardnog ulaza, koju zatim
prosledjuje detetu putem pajpa.
Dete cita tu liniju i ispisuje je na standardni izlaz.
*/

#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <sys/wait.h>
#include <string.h>

/* makro za proveru gresaka */
#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)
	
/* definicija strana pajpa i velicine bafera */
#define READ_END 	(0)
#define WRITE_END 	(1)
#define BUF_SIZE 	(256)	

int main(int argc, char** argv) {
	
	int pipeFds[2];
	
	/* BITNO:
	 * pajp se koristi kao mehanizam JEDNOSMERNE komunikacije
	 * izmedju detata i roditelja
	 * 
	 * za DVOSMERNU komunikaciju su potrebna DVA pajpa
	 */ 
	check_error(pipe(pipeFds) != -1, "pipe failed");
	
	/* nakon forka i child i parent imaju kopije i jedne i druge strane pajpa
	 * pa je svaki od njih duzan da zatvori onu stranu koja mu ne treba.
	 */
	pid_t childPid = fork();
	
	check_error(childPid != -1, "fork failed");
	
	if (childPid > 0) { // parent branch
		
		/* parent zatvara granu koja mu ne treba */
		close(pipeFds[READ_END]);
		
		char* line = NULL;
		size_t size = 0;
		
		/* ucitava se linija */
		check_error(getline(&line, &size, stdin) != -1, "getline failed");
		/* provera velicine */
		check_error(strlen(line) < BUF_SIZE, "message is too long");
		/* linija se salje childu */
		check_error(write(pipeFds[WRITE_END], line, strlen(line)) != -1, "write failed");
		
		/* zatvara se koriscena grana */
		close(pipeFds[WRITE_END]);
		free(line);
	}
	else { // child branch
		
		/* child zatvara granu koja mu ne treba */
		close(pipeFds[WRITE_END]);
		
		char line[BUF_SIZE];
		int readBytes = 0;
		
		/* cita se linija od parenta */
		check_error((readBytes = read(pipeFds[READ_END], line, BUF_SIZE)) != -1, "read failed");
		/* BITNO:
		 * linija se terminira nulom, jer read i write rade sa bajtovima
		 */ 
		line[readBytes] = '\0';
		
		/* linija se stampa na stdout */
		printf("Line received: %s\n", line);
		//write(STDOUT_FILENO, line, readbytes);
		
		/* child zatvara granu koju je koristio */
		close(pipeFds[READ_END]);
		
		exit(EXIT_SUCCESS);
	}
	
	/* BITNO:
	 * neophodno je sacekati childa da se ne bi sejali zombiji po
	 * sistemu 
	 */ 
	check_error(wait(NULL) != -1, "wait failed");
	
	exit(EXIT_SUCCESS);
}
