/*
Poziv programa:
./a.out numOfThreads

Program demonstrira rad sa POSIX nitima. Kreira se 
'numOfThreads' niti, pri cemu svaka od njih treba 
da ispise svoj TID, kao i id koji mu je dodeljen u
main-u. Na kraju, main nit treba da saceka na sve ostale.
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

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

/* neophodan je dodatni makro za obradu gresaka 
 * kod funkcija iz pthread.h
 * 
 * funkcije iz pthread.h vracaju 0 u slucaju uspeha i 
 * vrednost >0 u slucaju neuspeha. Vrednost vracena u slucaju
 * neuspeha odgovara errno vrednosti.
 * 
 * Prilikom obrade gresaka, errno se modifikuje samo u slucaju
 * kada se dogodi greska, jer dodela vrednosti errno-u se u programu
 * koji koristi tredove odmotava u poziv funkcije. Detalji u TLPI.
 */ 
#define osPthreadCheck(pthreadErr, userMsg) \
	do { \
		int _pthreadErr = pthreadErr; \
		if (_pthreadErr > 0) { \
			 errno = _pthreadErr; \
			 check_error(false, userMsg); \
		 }\
	} while (0)

const char* osUsage = "./hello_thread numOfThreads";	

/* tip koji predstavlja ulazni argument funkcije koja
 * implementira nit.
 * 
 * Treba obratiti paznju kada se koristi osnovni tipovi, da 
 * slanje osnovnih tipova direktno preko pokazivaca moze da napravi 
 * problem (endianness, portabilnost ...) 
 * Ako se koriste iskljucivo osnovni tipovi treba ih uokviriti u novi tip
 * ili koristiti ugradjene tipove koji garantuju portabilnost (intptr_t ...)
 */ 
typedef struct {
	
	int idx;
		
} osThreadFunctionArg;

/* tip koji predstavlja povratnu vrednost iz niti. 
 * Ako se vraca neki rezultat iz niti pomocu return mehanizma, najlakse
 * je uokviriti sve potrebne podatke u novi tip.
 */
typedef struct {
	
} osThreadFunctionRetType;

/* Funkcija implementira tred */
void* osThreadFunction(void* arg) {
	
	/* kastujemo prosledjeni argument u odgovarajuci tip */
	osThreadFunctionArg* farg = arg;
	
	/* stampamo poruku */
	printf("Hello from thread %d. Thread id %jd\n", farg->idx, (intmax_t)pthread_self());
	
	/* zavrsavamo nit */
	return NULL;
}

int main(int argc, char** argv) {
	
	/* provera argumenata */
	check_error(argc == 2, osUsage);
	
	int noOfThreads = atoi(argv[1]);
	/* tip podataka koji opisuje id niti je pthread_t
	 * 
	 * BITNO:
	 * nikada ne treba pretpostavljati koji se tip krije iz pthread_t, 
	 * vec uvek treba da se koristi tip pthread_t
	 */ 
	pthread_t* tids = malloc(noOfThreads * sizeof(pthread_t));
	check_error(tids != NULL, "allocation failed");
	
	/* alokacija struktura koje se koriste kao ulazni argumenti tredova */
	osThreadFunctionArg* data = malloc(noOfThreads * sizeof(osThreadFunctionArg));
	check_error(data != NULL, "allocation failed");
	
	int i = 0;
	for (i = 0; i < noOfThreads; i++) {
		/* inicijalizacija argumenta */
		data[i].idx = i;
		/* startovanje niti 
		 * 
		 * BITNO:
		 * isto kao i kod procesa, ne postoji garancija kada ce se koja
		 * nit startovati niti kojim redosledom
		 */
		osPthreadCheck(pthread_create(&tids[i], NULL, osThreadFunction, &data[i]), "pthread_create_failed");
	}
	
	/* kao i u slucaju procesa, treba pravilno obraditi ostatke svake
	 * zavrsene niti 
	 */ 
	for (i = 0; i < noOfThreads; i++) {
		/* BITNO:
		 * kada se koriste niti ne postoji mogucnost da se ceka
		 * bilo koja nit, vec uvek mora eksplicitno da se navede
		 * koja je nit koja se trenutno ceka.
		 * 
		 * BITNO 2:
		 * kod niti nema relacija parent-child, vec su sve niti u procesu
		 * braca i sestre, tj. svaka nit moze da saceka bilo koju drugu nit
		 * 
		 * BITNO 3:
		 * ako bilo koja nit pozove exit, to automatski znaci zavrsavanje 
		 * svih niti. Ako treba da se prekine izvrsavanje samo jedne niti
		 * potrebno je pozvati pthread_exit(...) ili return iz funckije
		 * koja implementira nit
		 * 
		 * BITNO 4:
		 * jedno joinovana nit ne sme se ponovo joinovati
		 */
		osPthreadCheck(pthread_join(tids[i], NULL), "pthread join failed");
	}
	
	/* oslobadjanje memorije */
	free(tids);
	free(data);
	
	exit(EXIT_SUCCESS);
}
