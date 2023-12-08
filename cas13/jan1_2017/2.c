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

static const char *os_Usage = "./2 pathToFile";

#define RD_END   (0)
#define WR_END 	 (1)
#define MAX_SIZE (8192)

int main(int argc, char** argv) {
	
	int pipeFds[2];
	
	check_error(argc == 2, os_Usage);
	check_error(pipe(pipeFds) != -1, "pipe failed");
	
	pid_t childPid = fork();
	check_error(childPid != -1, "fork failed");
	
	if (childPid == 0) { // child
		
		close(pipeFds[RD_END]);
		
		dup2(pipeFds[WR_END], STDOUT_FILENO);
		
		check_error(execlp("stat", "stat", "--format=%s", argv[1], NULL) != -1, "exec failed");
	}
	
	close(pipeFds[WR_END]);
	char buffer[MAX_SIZE];
	check_error(read(pipeFds[RD_END], buffer, MAX_SIZE) != -1, "read failed");
	
	close(pipeFds[RD_END]);
	
	int status = 0;
	check_error(waitpid(childPid, &status, 0) != -1, "wait failed");
	
	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) != EXIT_SUCCESS) {
			printf("Neuspeh\n");
			exit(EXIT_FAILURE);
		}
	}
	else {
		printf("Neuspeh\n");
		exit(EXIT_FAILURE);
	}
	
	int size;
	sscanf(buffer, "%d", &size);
	printf("%d\n", size);
	
	exit(EXIT_SUCCESS);
}
