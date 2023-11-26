/*
U ovom primeru se prvo kreira dete koje izvrsava komandu `ls -al`, a potom roditelj
ispisuje poruku na standardni izlaz. 
Postavlja se pitanje zasto se prvo ispise poruka roditelja,
a tek onda sadrzaj komande `ls -al`. Takodje, s obzirom da na kraju
nema prompt-a shell-a, da li to znaci da smo se zaglavili negde?

Razlog tome je sto nismo uradili wait i desice se da roditelj prvi
zavrsi svoj rad (ispise poruku i exit-uje). Kao odgovor na to shell 
ispise prompt na terminalu. Nakon toga dete poziva `ls -al` i ispisuje
se sadrzaj na stdout koji je vezan za isti terminal za koji su vezani
shell i roditeljski proces. Nakon toga mozemo uneti novu komandu shell-u,
jer su i roditelj i dete zavrsili rad, samo sto prompt ne stoji u terminalu.

Resenje je da se doda wait pre ispisa poruke roditelja.
*/

#include <stdio.h>
#include <stdlib.h>
       #include <sys/types.h>
       #include <unistd.h>
       #include <sys/types.h>
       #include <sys/wait.h>
#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

int main(int argc, char** argv) {
  check_error(argc == 1, "args");

	pid_t childPid = fork();
  check_error(childPid != -1, "fork");

	if (childPid == 0){ // DETE
		check_error(-1 != execlp("ls", "ls", "-al", NULL), "execl");
	}

	printf("Hello from parent\n");

	exit(EXIT_SUCCESS);
}
