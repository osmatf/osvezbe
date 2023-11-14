/*
Program demonstrira kreiranje novih procesa. Nakon kreiranja,
roditelj treba da ispise svoj PID i PID svog deteta, a dete
svoj PID i PID svog roditelja. Nakon toga, oba procesa ispisuju istu poruku.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

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
	
	/* novi proces se kreira pomocu funkcije fork
	 * rezultat forka su dve identicne kopije polaznog adresnog prostora
	 * 
	 * BITNO:
	 * nakon vracanja iz fork poziva ne postoji garancija koji proces
	 * prvi dobija procesor. Nikada se ne oslanjati na tako nesto prilikom
	 * sinhronizacije procesa 
	 */
	pid_t childPid = fork();
	
	/* neophodna obrada gresaka */
	check_error(childPid != -1, "fork failed");
	
	/* u slucaju da povratna vrednost > 0 radi se o roditeljskoj grani */
	if (childPid > 0) { // parent branch
		
		/* getpid -> ID pozivajuceg procesa
		 * getppid -> ID parenta 
		 */ 
		printf("Hello from parent. My pid is %jd, child pid is %jd\n", \
				(intmax_t)getpid(), (intmax_t)childPid);
	}
	/* inace, radi se o grani deteta */
	else { // child branch
		
		/* sleep je ovde ilustracija, ali ne NIKADA ne koristi za
		 * sinhronizaciju procesa 
		 */
		//sleep(5);
		printf("Hello from child. My pid is %jd, parent pid is %jd\n", \
				(intmax_t)getpid(), (intmax_t)getppid());
	}
	
	printf("We both execute this\n");
	
	exit(EXIT_SUCCESS);
}
