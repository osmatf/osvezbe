/*
Poziv programa:
./a.out

Program demonstrira rad sa "odvojenim" nitimaa. Kreira se 
nit koja ce se izvrsavati u pozadini, koja spava na 2 sekunde,
a potom ispisuje poruku.
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

#define osPthreadCheck(pthreadErr, userMsg) \
	do { \
		int _pthreadErr = pthreadErr; \
		if (_pthreadErr > 0) { \
			 errno = _pthreadErr; \
			 check_error(false, userMsg); \
		 }\
	} while (0)

const char* osUsage = "./hello_detach";	
	
void osErrorFatalImpl(const char* userMsg, const char* fileName, const char* functionName, const int lineNum);

void* osThreadFunction(void* arg) {
	
	/* varamo kompilator da koristimo argument */
	(void)arg;
	
	/* uspavljujemo nit da bismo protok vremena prilikom izracunavanja */
	sleep(2);
	
	printf("Hello from detached thread \n");
	
	return NULL;
}

int main(int argc, char** argv) {
	
	/* promenljiva koja cuva ID niti */
	pthread_t backgroundThread;
	/* startujemo nit */
	osPthreadCheck(pthread_create(&backgroundThread, NULL, osThreadFunction, NULL), "pthread create failed");
	/* ako ne zelimo da cekamo nit (join), potrebno je da je detach-ujemo,
	 * tj. da je prebacimo u pozadinu
	 * 
	 * Nit koja je detachovana nikad ne sme da bude joinovana
	 */ 
	osPthreadCheck(pthread_detach(backgroundThread), "pthread detach failed");	
	
	/* bez ovog uspavljivanja, program bi odmah pozvao exit i time
	 * zavrsio i izvrsavanje detachovane niti
	 * 
	 * iskomentarisati naredbu i uveriti se
	 */ 
	sleep(5);
	
	/* jednom detachovana nit se nikada ne ceka 
	 * 
	 * otkomentarisati i uveriti da se prijavljuje greska
	 */
	//osPthreadCheck(pthread_join(backgroundThread, NULL), "join failed");
	exit(EXIT_SUCCESS);
}
