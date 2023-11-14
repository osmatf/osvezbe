/*
Program demonstrira kreiranje novih procesa. Nakon kreiranja,
roditelj treba da ispise svoj PID i PID svog deteta, a dete
svoj PID i PID svog roditelja. Nakon toga, oba procesa ispisuju istu poruku.
Dodatno, dete poziva exit da bi zavrsilo izvrsavanje, dok roditelj ceka 
da dete zavrsi svoj rad.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <sys/wait.h>

/* makro za proveru gresaka */
#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

int main(int argc, char** argv) {
	
	pid_t childPid = fork();
	
	check_error(childPid != -1, "fork failed");
	
	if (childPid > 0) { // parent branch
		
		printf("Hello from parent. My pid is %jd, child pid is %jd\n", \
				(intmax_t)getpid(), (intmax_t)childPid);
		
		/* neophodno je sacekati da se child zavrsi, jer se u 
		 * suprotnom seju zombi procesi koji bespotrebno zagusuju 
		 * sistem
		 * 
		 * BITNO:
		 * nakon svakog forka, neophodno je sacekati da se dete zavrsi
		 */ 		
		check_error(wait(NULL) != -1, "wait failed");
	}
	else { // child branch
		
		//sleep(5);
		printf("Hello from child. My pid is %jd, parent pid is %jd\n", \
				(intmax_t)getpid(), (intmax_t)getppid());
		exit(EXIT_SUCCESS);
	}
	
	printf("Only parent executes this\n");
	
	exit(EXIT_SUCCESS);
}
