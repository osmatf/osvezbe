/*
Poziv programa:
./sync_atomic file0 file1 file2 ...

Kao argumenti komandne linije zadaju se
putanje do fajlova koji sadrze cele brojeve.
Potrebno je ispisati ukupnu sumu svih brojeva
u tim fajlovima. Obezbediti da svaki fajl obradjuje 
tacno jedna nit, kao i zastitu kriticne sekcije putem
interfejsa stdatomic.h.
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

/* zaglavlje neophodno ako koristimo atomicne operacije */
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

const char* osUsage = "./sync_atomic file0 file1 file2 ...";	

/* promenljiva predstavlja globalnu sumu 
 * kojoj konkurentno pristupaju sve pokrenute niti
 * 
 * pristup ovoj promenljivoj je ocigledno kriticna sekcija
 * i nju stitimo tako sto je definisemo kao atomicnu i 
 * nad njom vrsimo iskljucivo atomicne operacije
 */
static atomic_int globalSum;

/* funkcija implementira nit */
void* osThreadFunction(void* arg) {
	
	char* path = arg;
	/* BITNO:
	 * fajlovi se uvek otvaraju u nitima, a nikada pre njih
	 * niti dele sve fajl deskriptore, pa bi otvaranje fajla pre
	 * pokretanja niti dovelo do problema prilikom izvrsavanja 
	 */ 
	FILE* f = fopen(path, "r");
	check_error(f != NULL, "file open failed");
	
	int sum = 0; /* lokalna suma otvorenog fajla */
	int current; /* trenutno ucitani broj */
	/* dok god mozemo ucitavamo broj */
	while (fscanf(f, "%d", &current) == 1) {
		/* i dodajemo ga na lokalnu sumu */
		sum += current;
		/* dodavanje ucitanog broja na globalnu sumu je neispravno
		 * jer se time ponistava koncept paralelnog izvrsavanja
		 */ 
		// atomic_fetch_add(&globalSum, current);
	}
	/* proverava se da li greska uzrok izlaska iz while petlje */
	check_error(ferror(f) == 0, "ferror");
	
	/* ispravno resenje je dodavanje kompletne sume fajla na 
	 * globalnu sumu, cime se postize minimalno provodjenje vremena
	 * u kriticnoj sekciji
	 */ 
	atomic_fetch_add(&globalSum, sum);
	
	/* zatvaranje fajla */
	fclose(f);
	
	return NULL;
}

int main(int argc, char** argv) {
	
	check_error(argc >= 2, osUsage);
	
	/* alokacija prostora za promenljive koje sadrza ID niti */
	int noOfThreads = argc - 1;
	pthread_t* tids = malloc(noOfThreads * sizeof(pthread_t));
	check_error(tids != NULL, "allocation failed");
	
	/* BITNO:
	 * pre upotrebe neophodno je eksplicitno inicijalizovati
	 * globalnu sumu
	 */
	atomic_init(&globalSum, 0);
	
	/* startujemo niti */
	int i;
	for (i = 0; i < noOfThreads; i++) {
		
		osPthreadCheck(pthread_create(&tids[i], NULL, osThreadFunction, argv[i+1]), "pthread create failed");
	}
	
	/* sacekamo da se sve niti zavrse */
	for (i = 0; i < noOfThreads; i++) {
		
		osPthreadCheck(pthread_join(tids[i], NULL), "pthread join failed");
	}
	
	/* ispisemo sumu fajlova */
	printf("Sum of numbers: %d\n", atomic_load(&globalSum));
	
	/* oslobodimo memoriju */
	free(tids);
		
	exit(EXIT_SUCCESS);
}
