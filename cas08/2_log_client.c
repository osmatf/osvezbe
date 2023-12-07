/*
Poziv programa:
./log_client pathFile

Program ucitava objekat deljene memorije na putanji 'pathFile'
a potom u petlji ucitava poruke iz tog objekta i ispisuje ih
na svoj standardni izlaz. Kada se unese poruka "quit", 
program treba da zavrsi sa radom.
Posto vise procesa moze istovremeno citati/pisati iz tog objekta
deljene memorije, potrebno je sinhronizovati citanje i pisanje 
pomocu semafora. 
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>
#include <strings.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

#define MSG_LEN (256)
#define LOCAL_SEM (0)
#define PROC_SEM (!(LOCAL_SEM))

typedef struct {
	char buf[MSG_LEN];
	sem_t safeToRead;
	sem_t safeToWrite;
} osMemoryBlock;	

static char* osUsage = "./log_client pathFile";


void* osGetMemoryBlock(const char* filePath, unsigned* size) {
	
	int memFd = shm_open(filePath, O_RDWR, 0);
	check_error(memFd != -1, "shm open failed");
	
	struct stat fInfo;
	check_error(fstat(memFd, &fInfo) != -1, "stat failed");
	*size = fInfo.st_size;
	
	void *addr;
	check_error((addr = mmap(0, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap failed");
	
	close(memFd);
	
	return addr;
}

int main(int argc, char** argv) {
	
	check_error(argc == 2, osUsage);
	
	/* cita se objekat deljeme memorije */
	unsigned size;
	osMemoryBlock *pMsgBuf = osGetMemoryBlock(argv[1], &size);
	
	/* lokalni bafer */
	char localBuf[MSG_LEN];
	do {
		
		/* cekamo na semaforu koji stiti operaciju citanja
		 * i na taj nacin sinhronisemo pristup baferu za razmenu
		 * poruka u deljenoj memoriji
		 * 
		 * IZUZETNO VAZNO:
		 * sinhronisu se SAMO i ISKLJUCIVO deljeni resursi i promenljive. 
		 * 
		 * sve promenljive koje nisu deljene se NIKADA ne sinhronisu i njihovo
		 * pojavljivanje u kriticnoj sekciji je ozbiljna greska, jer 
		 * znacajno umanjuje ili u potpunosti ponistava ubrzanje koje se
		 * dobija paralelizacijom
		 */
		check_error(sem_wait(&pMsgBuf->safeToRead) != -1, "sem wait failed");
		
		/* BITNO:
		 * samo pristup, tj. upotreba deljene promenljive se 
		 * sinhronise
		 */
		strcpy(localBuf, pMsgBuf->buf);
		
		/* obavestavamo drugi proces da je bezbedno da upise novu poruku
		 * u deljenu memoriju
		 */
		check_error(sem_post(&pMsgBuf->safeToWrite) != -1, "sem post failed");
		
		/* BITNO:
		 * lokalna operacija se nikada ne sinhronise
		 */ 
		printf("Received message: '%s'\n", localBuf);
		
	} while(strcasecmp(localBuf, "quit"));
	
	/* izmene memorijsko bloka se trajno snimaju na disku, tj.
	 * u fajlu koji predstavlja deljenu memoriju 
	 */
	check_error(munmap(pMsgBuf, size) != -1, "munmap failed");
	
	exit(EXIT_SUCCESS);
}
