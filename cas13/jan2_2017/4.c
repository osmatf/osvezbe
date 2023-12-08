#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <errno.h>
#include <pthread.h>

#include <sys/mman.h>
#include <semaphore.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

const char* osUsage = "./4 pathToFile factor";	

#define MAX_STR_LEN (256)
	
typedef struct {
	sem_t inDataReady;
	sem_t outDataReady;
	char s[MAX_STR_LEN];
} OsInputData;

void* osGetMemoryBlock(char* filePath, int* size) {
	
	int memFd = shm_open(filePath, O_RDWR, 0600);
	check_error(memFd != -1, "shm_open faield");
	
	struct stat fInfo;
	check_error(fstat(memFd, &fInfo) != -1, "fstat failed");
	*size = fInfo.st_size;
	
	void* addr;
	
	check_error((addr = mmap(0, *size, PROT_READ|PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap failed");
	
	close(memFd);
	
	return addr;
}

int main(int argc, char** argv) {
	
	check_error(argc == 3, osUsage);
	
	int size = 0;
	OsInputData* data = osGetMemoryBlock(argv[1], &size);
	
	int factor = atoi(argv[2]);
	
	check_error(sem_wait(&(data->inDataReady))!=-1, "sem wait failed");
	
	int i = 0;
	for (i = 0; data->s[i] != 0; i++) {
		data->s[i] = (data->s[i] + factor) % 128;
	}
	
	check_error(sem_post(&(data->outDataReady)) != -1, "sem post failed");
	
	check_error(munmap(data, size) != -1, "maunmap failed");
	
	exit(EXIT_SUCCESS);
}
