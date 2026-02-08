#define _POSIX_C_SOURCE 200809L
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#include <sys/wait.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

static const char *os_Usage = "Usage: ./1 file0 file1 ... fileN";

#define RD_END 0
#define WR_END 1
#define MAX_SIZE (1024)

int main(int argc) {
	
	/* kreiramo pipe za komunikaciju */
	int pipeFds[2];
	check_error(pipe(pipeFds) != -1, "pipe failed");
	
	/* forkujemo novi proces */
	pid_t childPid = fork();
	
	check_error(childPid != -1, "fork failed");
	
	if (childPid > 0) { // parent
			
		/* zatvaramo stranu koja nam ne treba */
		close(pipeFds[WR_END]);
		
		/* otvaramo stream nad file deskriptorom */
		FILE* f = fdopen(pipeFds[RD_END], "r");
		check_error(f != NULL, "fdopen failed");
		
		char* line = NULL;
		size_t lineLen = 0;
		int lineNum = 0;
		
		/* ucitavamo liniju po liniju */
		while (getline(&line, &lineLen, f) >= 0) {
			
			/* ignorisemo prvu liniju */
			if (lineNum >= 1) {
				
				char mode[MAX_SIZE];
				int num;
				char userName[MAX_SIZE];
				char group[MAX_SIZE];
				int size;
				/* citamo sve do cetvrte reci u liniji koja predstavlja velicinu */
				sscanf(line, "%s%d%s%s%d", mode, &num,userName, group, &size);
				
				/* stampamo velicinu */
				printf("%d\n", size);
			}
			
			lineNum++;
		}
		/* oslobadjamo liniju */
		free(line);
		
		/* zatvaramo stream */
		fclose(f);
	}
	else { // child
		
		/* zatvaramo stranu koja nam ne treba */
		close(pipeFds[RD_END]);
		
		/* preusmeravamo izlaz da ide na pajp ka dete procesu */
		check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2 failed");
		check_error(freopen("/dev/null", "w", stderr) != NULL, "freopen failed");
		
		/* execujemo ls */
		check_error(execlp("ls", "ls", "-l", NULL) != -1, "exec failed");
		
		exit(EXIT_FAILURE);
	}
	
	/* cekamo dete proces da zavrsi sa izvrsavanjem */
	check_error(wait(NULL) != -1, "wait failed");

	exit(EXIT_SUCCESS);
}
