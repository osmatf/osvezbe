#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#include <signal.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

static const char *os_Usage = "";

int sigusr1Count = 0;
int sigusr2Count = 0;
bool shouldTerminate = false;

void signalHandler(int signum) {
	
	switch (signum) {
		
		case SIGUSR1:
			sigusr1Count++;
			break;
		case SIGUSR2:
			sigusr2Count++;
			break;
		case SIGTERM:
			shouldTerminate = true;
			break;
		default:
			break;
	}
}

int main(int argc, char** argv) {
	
	check_error(signal(SIGUSR1, signalHandler) != SIG_ERR, "signal handler setuo failed");
	check_error(signal(SIGUSR2, signalHandler) != SIG_ERR, "signal handler setuo failed");
	check_error(signal(SIGTERM, signalHandler) != SIG_ERR, "signal handler setuo failed");
	
	//fprintf(stderr, "PID: %jd\n", (intmax_t)getpid());
	
	do {
		pause();
	} while(!shouldTerminate);
	
	printf("%d %d\n", sigusr1Count, sigusr2Count);
	
	exit(EXIT_SUCCESS);
}
