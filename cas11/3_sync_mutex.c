/*
Poziv programa:
./sync_mutex file0 file1 file2 ...

Kao argumenti komandne linije zadaju se
putanje do fajlova koji sadrze cele brojeve.
Potrebno je ispisati ukupnu sumu svih brojeva
u tim fajlovima. Obezbediti da svaki fajl obradjuje 
tacno jedna nit, kao i zastitu kriticne sekcije.
*/

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

#include <stdatomic.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

#define osPthreadCheck(pthreadErr, userMsg) \
	do { \
		int _pthreadErr = pthreadErr; \
		if (_pthreadErr > 0) { \
			 errno = _pthreadErr; \
			 check_error(false, userMsg); \
		 }\
	} while (0)

const char* osUsage = "./sync_mutex file0 file1 file2 ...";	
	
/* struktura predstavlja tip podataka koji se koristi prilikom sinhronizcije */
typedef struct {
	int data; /* globalna suma. Kriticna sekcija, tj. podataka kome treba sinhrono pristupati */
	pthread_mutex_t lock; /* mutex kojim se stiti pristup kriticnoj sekciji */
} osSafeInt;

osSafeInt globalSum; /* konkretan primerak strukture */

/* funkcija implementira nir */
void* osThreadFunction(void* arg) {
	
	char * path = arg;
	/* otvaranje fajla */
	FILE* f = fopen(path, "r");
	check_error(f != NULL, "file open failed");
	
	/* racunanje lokalne sume fajla */
	int sum = 0;
	int current = 0;
	while (fscanf(f, "%d", &current) == 1) {
		
		sum += current;
	}
	
	check_error(ferror(f) == 0, "read error");
	
	/* BITNO:
	 * mutex-om se stiti samo pristup kriticnoj sekciji
	 * 
	 * JOS BITNIJE:
	 * zakljucavanje cele funkcije je ozbiljna greska, kojom se 
	 * ponistava konkurentnost izvrsavanja. Na taj nacin, program se svodi
	 * na sekvencijalno citanje fajlova 
	 */ 
	osPthreadCheck(pthread_mutex_lock(&globalSum.lock), "mutex lock failed");
	globalSum.data += sum;
	osPthreadCheck(pthread_mutex_unlock(&globalSum.lock), "mutex unlock failed");
	
	/* zatvaranje fajlova */
	fclose(f);
	
	return NULL;
}

int main(int argc, char** argv) {
	
	check_error(argc >= 2, osUsage);
	
	/* BITNO:
	 * pre prve upotrebe potrebno je inicijalizovati mutex 
	 * i deljenu promenljivu
	 */
	globalSum.data = 0;
	osPthreadCheck(pthread_mutex_init(&globalSum.lock, NULL), "mutex init failed");
	
	/* alokacija prostora za promenljive koje cuvaju ID niti */
	int noOfThreads = argc - 1;
	pthread_t* tids = malloc(noOfThreads * sizeof(pthread_t));
	check_error(tids != NULL, "allocation failed");
	
	/* startovanje niti */
	int i = 0;
	for (i = 0; i < noOfThreads; i++) {
		osPthreadCheck(pthread_create(&tids[i], NULL, osThreadFunction, argv[i + 1]), "pthread create failed");
	}
	
	/* cekanje da se niti zavrse */
	for (i = 0; i < noOfThreads; i++) {
		osPthreadCheck(pthread_join(tids[i], NULL), "pthread join failed");
	}
	
	/* stampanje globalne sume */
	printf("Sum of numbers: %d\n", globalSum.data);
	
	/* pslobadjanje memorije */
	free(tids);
	
	/* unistavanje mutex-a nakon upotrebe */
	osPthreadCheck(pthread_mutex_destroy(&globalSum.lock), "mutex destroy failed");
		
	exit(EXIT_SUCCESS);
}
