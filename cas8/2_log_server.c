/*
Poziv programa:
./log_server pathFile

Program kreira objekat deljene memorije na putanji 'pathFile'
a potom u petlji ucitava poruke od korisnika sa standardnog ulaza,
koje zatim upisuje u objekat deljene memorije. Kada se unese poruka
"quit", program treba da zavrsi sa radom.
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

#define MSG_LEN (256) 			/* maksimalna velicina poruke u bloku memorije */
#define LOCAL_SEM (0)			/* lokalni semafor - koristi se za sync tredova */
#define PROC_SEM (!LOCAL_SEM)	/* globalni semafor - koristi se za sync procesa */

/* struktura koja opisuje sadrzaj fajla deljene memorije
 * 
 * IZUZETNO VAZNO:
 * radosled polja u svim procesima koji koriste ISTI semafor za sync
 * ISTOG resursa MORA biti jednak u svim procesima, inace program nece
 * raditi kako treba
 */
typedef struct {
	char buf[MSG_LEN];
	sem_t safeToRead;
	sem_t safeToWrite;
} osMemoryBlock;

static char* osUsage = "./log_server pathFile";

void *osCreateMemoryBlock(const char* filePath, unsigned size) {
	
	int memFd = shm_open(filePath, O_RDWR | O_CREAT, 0600);
	check_error(memFd != -1, "shm_open failed");
	
	check_error(ftruncate(memFd, size) != -1, "ftruncate failed");
	
	void* addr;
	check_error((addr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap failed");
	
	close(memFd);
	
	return addr;
}

int main(int argc, char** argv) {
	
	check_error(argc == 2, osUsage);
	
	/* kreira se memorijski blok */
	osMemoryBlock* pMsgBuf = osCreateMemoryBlock(argv[1], sizeof(osMemoryBlock));
	
	/* inicijalizacija semafora
	 * 
	 * IZUZETNO VAZNO:
	 * samo JEDAN proces radi inicijalizaciju semafora
	 * svi ostali procesi samo koriste semafore
	 * 
	 * broj kojim se inicijalizuje semafor treba da bude jednak 
	 * ukupnom broju dostupnih resursa. Ako nema dostupnih resursa
	 * semafor se inicijalizuje nulom
	 */ 
	check_error(sem_init(&pMsgBuf->safeToRead, PROC_SEM, 0) != -1, "read sem init failed");
	check_error(sem_init(&pMsgBuf->safeToWrite, PROC_SEM, 1) != -1, "write sem init failed");
	
	/* lokalni bafer za ucitavanje poruka */
	char localBuf[MSG_LEN];
	
	do {
		/* ucitavamo poruku od korisnika */
		scanf("%s", localBuf);
		
		/* cekamo na semaforu koji stiti operaciju pisanja 
		 * i na taj nacin sinhronizujemo pristup baferu za razmenu
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
		check_error(sem_wait(&pMsgBuf->safeToWrite)!=-1, "sem wait failed");
		
		/* BITNO:
		 * samo pristup, tj. upotreba deljene promenljive se 
		 * sinhronise
		 */ 
		strcpy(pMsgBuf->buf, localBuf);
		
		/* obavestavamo drugi proces da je postalvjena nova poruka
		 * u deljenoj memoriji
		 */
		check_error(sem_post(&pMsgBuf->safeToRead) != -1, "sem post failed");
		
	} while(strcasecmp(localBuf, "quit"));
	
	/* izmene memorijsko bloka se trajno snimaju na disku, tj.
	 * u fajlu koji predstavlja deljenu memoriju 
	 */
	check_error(munmap(pMsgBuf, sizeof(osMemoryBlock)) != -1, "munmap failed");
	/* brise se objekat deljene memorije 
	 * BITNO:
	 * samo jedan proces ovo radi
	 */
	check_error(shm_unlink(argv[1]) != -1, "shm unlink failed");
	
	exit(EXIT_SUCCESS);
}
