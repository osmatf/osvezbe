/*
Poziv programa:
./a.out 

Program prvo kreira dete proces, a potom u beskonacnoj petlji cita 
liniju po liniju sa standardnog ulaza i prosledjuje ih detetu.
Dete treba da cita te linije i ispise ih na standardni izlaz kako pristizu.

Program prestaje sa radom kada stigne signal SIGINT. Tada roditeljski proces
treba da posalje detetu poruku "kraj", pri cemu dete tada izlazi iz petlje i
zavrsava sa radom. Roditeljski proces takodje tada izlazi iz petlje i zavrsava sa radom.

NAPOMENA1: sta se desi ukoliko posaljemo SIGINT tako sto pritisnemo Control-C?
I roditelj i dete dobiju isti signal i dete bude terminisano jer nema podesen signal handler.
Zato slati SIGINT roditelju preko komande `kill`.
Kako bismo se uverili da i roditelj i dete dobiju signal? Postavimo drugi signal handler
samo za dete sa nekim ispisom i proverimo da li se taj ispis dogodi.

NAPOMENA2: kada roditelj izvrsi svoj signal handler, izvrsavanje koda ce se verovatno vratiti
u funckiju getline koja ceka na novu liniju. Zbog toga moramo uneti jos nesto (barem samo enter)
da bi se program odglavio i zavrsio sa radom.
*/

#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

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

/* S obzirom na to da u signal handler-u moramo pozvati write() i pisati
 * na write kraj pajpa, taj kraj moramo cuvati kao globalnu promenljivu
 */
int write_end_of_pipe = -1;

bool shouldTerminate = false;

void obradi_signal(int signum){
  printf("Stigao interrupt\n");
  check_error(write(write_end_of_pipe, "kraj", 4) != -1, "write");
  shouldTerminate = true;
}


int main(int argc, char** argv) {
	
  int pipeFds[2];
	check_error(pipe(pipeFds) != -1, "pipe failed");

  write_end_of_pipe = pipeFds[WRITE_END];
	
  pid_t childPid = fork();
	check_error(childPid != -1, "fork failed");

	if (childPid > 0) { // parent branch
		close(pipeFds[READ_END]);
		
    check_error(signal(SIGINT, obradi_signal) != SIG_ERR, "signal");
		char* line = NULL;
		size_t size = 0;
		
    while(!shouldTerminate){
      check_error(getline(&line, &size, stdin) != -1, "getline failed");
      check_error(strlen(line) < BUF_SIZE, "message is too long");
      check_error(write(pipeFds[WRITE_END], line, strlen(line)) != -1, "write failed");
    }
		close(pipeFds[WRITE_END]);
		free(line);
	}
	else { // child branch
		close(pipeFds[WRITE_END]);

		char line[BUF_SIZE];
		int readBytes = 0;
		
    while(1){
      check_error((readBytes = read(pipeFds[READ_END], line, BUF_SIZE)) != -1, "read failed");
      line[readBytes] = '\0';
      printf("Line: %s\n", line);
      if(strcmp(line, "kraj") == 0){
        printf("Kraj\n");
        break;
      }
      printf("Line received: %s\n", line);
    }

    close(pipeFds[READ_END]);
		
		exit(EXIT_SUCCESS);
	}
	check_error(wait(NULL) != -1, "wait failed");
	
	exit(EXIT_SUCCESS);
}
