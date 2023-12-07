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

#define MAX_DATA_LEN (1024) 
   
typedef struct {
	sem_t ready;
	sem_t sorted;
	double data[MAX_DATA_LEN];
	unsigned numOfElems;
} OsSharedDoubArray;
    
static const char *os_Usage = "./3 shm_name";

void *osGetMemoryBlock(const char *name, int *blockSize);

int main(int argc, char** argv) {
	
	check_error(argc == 2, os_Usage);
	
	 /**
     * Citamo objekat deljene memorije (mapiranje)
     * */
    int blockSize = 0;
    OsSharedDoubArray* data = osGetMemoryBlock(argv[1], &blockSize);
    
    /* cekamo da podaci budu upisani */
    check_error(sem_wait(&data->ready) != -1, "sem wait failed");
    
    /* sortiramo podatke */
    int i = 0;
    int j = 0;
    for (i = 0; i < data->numOfElems; i++) {
		
		for (j = 0; j < data->numOfElems; j++) {
		
			if (data->data[i] > data->data[j]) {
				
				double tmp = data->data[i];
				data->data[i] = data->data[j];
				data->data[j] = tmp;
			}
		}
	}
    
    /* obavestavamo da su podaci sortirani */
    check_error(sem_post(&data->sorted) != -1, "sem wait failed");
    
    /* brisemo virtuelno mapiranje */
    check_error(-1 != munmap(data, blockSize), "Unmapping memory failed");
	
	exit(EXIT_SUCCESS);
}

void *osGetMemoryBlock(const char *name, int *blockSize) {
    /**
     * Open shared memory object
     * */
    int memFd = shm_open(name, O_RDWR, 0);
    check_error(-1 != memFd, "Opening shared memory object");
    
    /**
     * Get size of memory block
     * */
    struct stat finfo;
    check_error(-1 != fstat(memFd, &finfo), "Getting shared memory info failed");
    *blockSize = finfo.st_size;
    
    /**
     * Map memory into process address space
     * */
    void *addr;
    check_error(MAP_FAILED != (addr = mmap(NULL, *blockSize, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)), "Mapping memory failed");
    close(memFd);
    return addr;
}
