#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include <signal.h>
#include <sys/wait.h>
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

#define ARRAY_MAX (1024)

typedef struct {
	sem_t dataProcessingFinished;
	int array[ARRAY_MAX];
	unsigned arrayLen;
} OsInputData;

void* getMemBlock(char* path, int* size)
{
	int memFd = shm_open(path, O_RDWR, 0600);
	check_error(memFd != -1, "shm_open");

	struct stat fInfo;
	check_error(fstat(memFd, &fInfo) != -1, "fstat");

	*size = fInfo.st_size;

	void* addr = mmap(path, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);

	check_error(addr != MAP_FAILED, "mmap");
	close(memFd);

	return addr;
}

int sig1 = 0;
int sig2 = 0;

void signalHandler(int signum)
{
	switch (signum){
		case SIGUSR1:
			sig1 = 1;
			break;
		case SIGUSR2:
			sig2 = 1;
			break;
		default:
			check_error(0, "signal handler");
	}

}

int main(int argc, char **argv){
	check_error(argc == 2, "argc");

	char* fpath = argv[1];

	int size;
	OsInputData* data = getMemBlock(fpath, &size);


	check_error(signal(SIGUSR1, signalHandler) != SIG_ERR, "sigusr1");
	check_error(signal(SIGUSR2, signalHandler) != SIG_ERR, "sigusr2");
	pause();

	if(sig1)
	{
		sig1 = 0;
		for(int i = 0; i <data->arrayLen; i++)
		{
			data->array[i] = (data->array[i]*(-1));
		}
	}
	else  if(sig2)
	{
		sig2 = 0;
		for(int i = 0; i < data->arrayLen; i++)
		{
			data->array[i] = 2*(data->array[i]);
		}
	}

	check_error(sem_post(&(data->dataProcessingFinished)) != -1, "sem post");

	munmap(data, size);
	exit(EXIT_SUCCESS);
}