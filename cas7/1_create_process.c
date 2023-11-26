/*
Poziv programa:
./create_process command [args]

Program kroz argumente komandne linije dobija komandu (sa argumentima)
koju treba pokrenuti. Potom kreira dete proces koje vrsi exec
prosledjene komande. Program treba da saceka na izvrsavanje deteta,
a zatim da ispise odgovarajucu poruku.
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

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)
	
#define READ_END 	(0)
#define WRITE_END 	(1)
#define BUF_SIZE 	(256)	

/*
Ako je poziv programa bio na primer:
./a.out /usr/bin/ls -a -l
onda ce niz stringova programVars biti:
{"/usr/bin/ls","ls","-a","-l", NULL}
*/
pid_t osCreateProcess(char** programVars) {
	/* BITNO:
	 * najcesci model koriscenja exec-a je u kombianciji sa forkom,
	 * tj. forkuju se procesi i onda se adresni prostor childa
	 * menja novim procesom uz pomoc exec sistemskog poziva
	 */ 
	pid_t childPid = fork();
	
	if (childPid < 0)
		return -1;
	
	if (childPid == 0) { // child branch
		
		/* u childu vrsimo exec i pokrecemo program koji korisnik zeli */
		check_error(execvp(programVars[0], programVars + 1) != -1, "exec failed");
		
		return 0; // never executed
	}
	else { // parent grana
		return childPid;
	}
}

void osCreateProcessFromArgv(int argc, char** argv) {
	
	/*
	 * pakujemo argumente na nacin na koji to execvp nalaze 
	 * ukoliko je program pozvan sa "./a.out /usr/bin/ls -a -l"
	 * programVars ce biti niz {"/usr/bin/ls", "ls", "-a", "-l", NULL}
	*/
	char** programVars = malloc((argc+1)*sizeof(char*));
	check_error(programVars != NULL, "allocation failed");
	
	programVars[0] = malloc(strlen(argv[1]) + 1);
	check_error(programVars[0] != NULL, "alocation failed");
	strcpy(programVars[0], argv[1]);
	
	char* nameCandidate = strrchr(programVars[0], '/');
	char* name = nameCandidate != NULL ? nameCandidate + 1 : programVars[0];
	programVars[1] = malloc(strlen(name) + 1);
	check_error(programVars[1] != NULL, "allocation failed");
	strcpy(programVars[1], name);
	
	int i = 0;
	for ( i = 2 ; i < argc; i++) {
		programVars[i] = malloc(strlen(argv[i]) + 1);
		check_error(programVars[i] != NULL, "allocation failed");
		strcpy(programVars[i], argv[i]);
	}
	programVars[i] = NULL;
	
	/* kreiramo novi proces */
	pid_t processHandle = osCreateProcess(programVars);
	
	check_error(processHandle != -1, "fork failed");
	
	/* cekamo da se child zavrsi i proveravamo status zavrsetka */
	int status = 0;
	check_error(waitpid(processHandle, &status, 0) != -1, "waitpid failed");
	
	if (WIFEXITED(status))
		printf("Process terminated successfully. Exit code: %d\n", WEXITSTATUS(status));
	else 
		printf("Process didn't terminate successfully\n");
		
	for (i = 0; i < argc + 1; i++)
			free(programVars[i]);
	free(programVars);
}

int main(int argc, char** argv) {
	
	check_error(argc >= 2, "./create_process command -[options]");
	
	osCreateProcessFromArgv(argc, argv);
	
	exit(EXIT_SUCCESS);
}
