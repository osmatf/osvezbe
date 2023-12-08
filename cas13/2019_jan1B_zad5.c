#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <ctype.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)
	
#define check_pthread(err, msg)\
	do{\
		int _err = err;\
		if(_err > 0){\
			errno = _err;\
			check_error(0, msg);\
		}\
	}while(0);
	

int main(int argc, char **argv){
	
	check_error(argc == 2, "argc");
	
	FILE* input = fopen(argv[1], "r+");
	check_error(input != NULL, "fopen");
	
	
	char buffer[256];
	while(fscanf(input, "%s", buffer) != EOF){
		
		int broj = atoi(buffer);
		if(broj >= 1000 && broj <= 9999){
			struct flock lock;
			int pos = ftell(input);
			int fd = fileno(input);
			
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = pos - 4;
			lock.l_len = 4;
			
			if(fcntl(fd, F_SETLK, &lock) != -1){
				check_error(fseek(input, pos - 4, SEEK_SET) != -1, "fseek");
				fprintf(input, "%s", "####");
			}
		}
		
	}
	//check_error(feof(input) == 0, "fscanf");
	
	fclose(input);
	exit(EXIT_SUCCESS);
}