/*
Program demonstrira upotrebu promenljivih nakon poziva fork().
Nakon kreiranja, dete dobija kopiju adresnog prostora roditelja,
ali je ta kopija nezavisna od originala, pa upotreba iste 
promenljive u detetu nema uticaja na tu promenljivu u roditelju (i obrnuto).
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
		
	int var = 5;
	
	pid_t childPid = fork();
	
	check_error(childPid != -1, "fork failed");
	
	if (childPid > 0) { // parent branch
		
		printf("Hello from parent. My pid is %jd, child pid is %jd\n", \
				(intmax_t)getpid(), (intmax_t)childPid);
	}
	else { // child branch
		
		/* child dobija svoju kopiju promenljive koju moze da menja kako
		 * zeli, bez ikakvog uticaja na promenljivu u parentu
		 */ 
		//sleep(5);
		printf("Hello from child. My pid is %jd, parent pid is %jd\n", \
				(intmax_t)getpid(), (intmax_t)getppid());
		var *= 2;
		printf("Var in child: %d\n", var);		
		
		exit(EXIT_SUCCESS);
	}
	
	/* ako nas zanima status zavrsetka potrebno je da ga uhvatimo
	 * iz wait funkcije tako sto joj prosledimo adresu promenljive
	 * 
	 * BITNO:
	 * status zavrsetka nije isto sto i exit code. exit code je deo 
	 * statusa zavrsetka
	 */ 
	int status;
	check_error(wait(&status) != -1, "wait failed");
	
	/* ako nam treba exit code, prvo treba da proverimo da li se program
	 * ispravno zavrsio 
	 */ 
	if (WIFEXITED(status))
		/* ako jeste, onda citamo status zavrsetka */
		printf("Process exited normally. Exit code: %d\n", WEXITSTATUS(status));
	else 
		printf("Process exited abnormally\n");
	
	printf("Only parent executes this\n");
	printf("Var in parent: %d\n", var);
	
	
	exit(EXIT_SUCCESS);
}
