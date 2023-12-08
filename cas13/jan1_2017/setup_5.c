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

#include <time.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

const char* osUsage = "./hello_thread numOfThreads";	

#define MAX_ARRAY 1024	
	
typedef struct {
	sem_t inDataReady;
	float array[MAX_ARRAY];
	unsigned arrayLen;
} OsInputData;

void* osCreateMemoryBlock(char* filePath, unsigned size) {
	
	int memFd = shm_open(filePath, O_RDWR | O_CREAT, 0600);
	check_error(memFd != -1, "shm_open faield");
	
	
	check_error(ftruncate(memFd, size) != -1, "ftruncate failed");
	
	void* addr;
	
	check_error((addr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap failed");
	
	close(memFd);
	
	return addr;
}

int main(int argc, char** argv) {
	
	check_error(argc == 2, osUsage);
	
	/* proces mora da kreira blok deljene memorije
	 * koji koristi Vas zadatak
	 */
	OsInputData* data = osCreateMemoryBlock(argv[1], sizeof(OsInputData));
	
	/* proces koji puni deljenu memoriju MORA da inicijalizuje
	 * semafor. 
	 * Proces koji koristi semafor, tj. vas zadatak, NE SME da inicijalizuje memoriju
	 * Visestruko inicijalizovanje semafora dovodi do nedefinisanog ponasanja
	 */
	 check_error(sem_init(&data->inDataReady, 1, 0) != -1, "sem init failed"); 
	
	/* seme generatora brojeva */
	srand(time(NULL));
	
	/* slucajan broj inicijalizuje broj elemenata */
	data->arrayLen = rand() % MAX_ARRAY;
	
	
	int i, j;
	for (i = 0; i < data->arrayLen; i++) {
		data->array[i] = (float)(rand()/(float)RAND_MAX);
	}
	
	/* za debug naci medijanu ovde, pa odstampati i uporediti rezultate 
	 * sa rezultatima vaseg programa ...
	 */
	
	
	/* postovanjem na semaforu obavestava se Vas zadatak da su podaci spremni */
	check_error(sem_post(&data->inDataReady) != -1, "sem post failed");
	
	check_error(munmap(data, sizeof(OsInputData)) != -1, "munmap failed");
	
	exit(EXIT_SUCCESS);
}
