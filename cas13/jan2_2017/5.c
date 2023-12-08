#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#include <pthread.h>
#include <string.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)
    
static const char *osUsage = "./5 filePath wordToCheck";

#define MAX_SIZE (256)

int main(int argc, char** argv) {
	
	check_error(argc == 3, osUsage);
	
	int fd = open(argv[1], O_RDONLY);
	check_error(fd != -1, "open failed");
	
	FILE* f = fdopen(fd, "r");
	check_error(f != NULL, "fdopen failed");
	
	int wordLength = strlen(argv[2]);
	
	char localBuffer[MAX_SIZE];
	while (!feof(f)) {
		
		fscanf(f, "%s", localBuffer);
		if (!strcmp(localBuffer, argv[2])) {
			
			struct flock lock;
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = ftell(f);
			lock.l_len = -wordLength;
			
			check_error(fcntl(fd, F_GETLK, &lock) != -1, "fcntl failed");
			
			long currentOffset = ftell(f) - wordLength;
			
			switch (lock.l_type) {
				case F_WRLCK:
					printf("%ld w\n", currentOffset);
					break;
				case F_RDLCK:
					printf("%ld r\n", currentOffset);
					break;
				case F_UNLCK:
					//printf("%ld u\n", currentOffset);
					break;
			}
		}
	}
	
	fclose(f);
	
	exit(EXIT_SUCCESS);
}
