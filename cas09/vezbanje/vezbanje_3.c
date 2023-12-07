#define _XOPEN_SOURCE 700
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
    
static const char *os_Usage = "./4 pathToFile";

int main(int argc, char** argv) {
	
	check_error(argc == 2, os_Usage);
	
	/* forkujemo novi proces */
	pid_t childPid = fork();
	check_error(childPid != -1, "fork failed");
	
	/* ako je child proces */
	if (childPid == 0) {
		
		/* execujemo gcc sa argumentom komandne linije */
		check_error(execlp("gcc", "gcc", argv[1], NULL) != -1, "exec failed");
	}
	
	/* sacekamo da se gcc izvrsi i uhvatimo exit status */
	int status = 0;
	check_error(waitpid(childPid, &status, 0) != -1, "waitpid failed");
	
	/* izvucemo exit kod */
	if (WIFEXITED(status)) {
		int exitCode = WEXITSTATUS(status);
	
		/* ako nije uspesno zavrseno */
		if (exitCode != EXIT_SUCCESS) {
		
			/* zavrsimo parenta sa istim kodom koji je imao i gcc */
			exit(exitCode);
		}
		
		/* u suprotnom ispisemo velicinu dobijenog izvrsnog fajla */
		struct stat fInfo;
		check_error(stat("a.out", &fInfo) != -1, "stat failed");
		printf("%jd\n", (intmax_t)fInfo.st_size);
	}
	
	exit(EXIT_SUCCESS);
}
