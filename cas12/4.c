#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

#define RD_END 0
#define WR_END 1
#define MAX_SIZE (8192)

int main (int argc, char** argv) {
    
    int parentToChild[2];
    int childToParent[2];
    
    check_error(pipe(parentToChild) != -1, "pipe failed");
    check_error(pipe(childToParent) != -1, "pipe failed");
    
    pid_t childPid = fork();
    
    check_error(childPid != -1, "fork failed");
		
	if (childPid == 0) 
	{
		close(parentToChild[WR_END]);
		close(childToParent[RD_END]);
		check_error(dup2(parentToChild[RD_END], STDIN_FILENO) != -1, "dup failed");
		check_error(dup2(childToParent[WR_END], STDOUT_FILENO) != -1, "dup failed");
		close(parentToChild[RD_END]);
		close(childToParent[WR_END]);
		check_error(execlp("tr","tr", "a", "A", NULL) != -1, "exec failed");
	}
	else {
		
		close(parentToChild[RD_END]);
		close(childToParent[WR_END]);
		
		int fd = open(argv[1], O_RDONLY);
		check_error(fd != -1, "open failed");
			
		char buffer[MAX_SIZE];
		memset(buffer, 0, MAX_SIZE);
		int readBytes = 0;
		
		check_error((readBytes = read(fd, buffer, MAX_SIZE)) != -1, "read failed");
		
		check_error(write(parentToChild[WR_END], buffer, MAX_SIZE) != -1, "write failed");
		
		check_error((readBytes = read(childToParent[RD_END], buffer, MAX_SIZE)) != -1, "read failed");
		
		close(parentToChild[WR_END]);
		close(childToParent[RD_END]);
		
		int status = 0;
		check_error(waitpid(childPid, &status, 0) != -1, "waitpid failed");
		
		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) != EXIT_SUCCESS)
				printf("Neuspeh\n");
			else
				check_error(write(STDOUT_FILENO, buffer, readBytes) != -1, "write failed");
		}
		else {
			printf("Neuspeh\n");
		}
	}
	
	exit(EXIT_SUCCESS);
}
