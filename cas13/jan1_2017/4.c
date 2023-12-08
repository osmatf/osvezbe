#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

static const char *os_Usage = "./4 pathToFile a b";

int main(int argc, char** argv) {
	
	check_error(argc == 4, os_Usage);
	
	int a = atoi(argv[2]);
	int b = atoi(argv[3]);
	
	int fd = open(argv[1], O_RDWR);
	check_error(fd != -1, "open failed");
	
	struct flock fLock;
	fLock.l_type = F_WRLCK;
	fLock.l_whence = SEEK_SET;
	fLock.l_start = a;
	fLock.l_len = b;
	
	check_error(fcntl(fd, F_GETLK, &fLock) != -1, "flock failed");
	switch (fLock.l_type) {
		case F_UNLCK:
			printf("Unlocked\n");
			break;
		case F_RDLCK:
			printf("Shared lock\n");
			break;	
		case F_WRLCK:
			printf("Exclusive lock\n");
			break;
		default:
			break;
	} 
	
	close(fd);
	
	exit(EXIT_SUCCESS);
}
