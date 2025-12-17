/* Program ilustruje razne funkcije za rad sa vremenom */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <sys/time.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

int main(int argc, char** argv) {
	/* funkcija time vraca broj sekundi od epohe */
	time_t now = time(NULL);		
	/* funkcija ctime kreira prikaz citljiv prikaz vremena
	 * BITNO: 
	 * novi red je sastavni deo kreiranog stringa
	 */ 
	printf("%s\n", ctime(&now)); 

	/* merenje vremene u sekundama od epohe je prilicno neupotrebljivo
	 * Sa tim u vezi, sistem omogucava razbijanje tog vremena na citljiviji oblik.
	 * 
	 * Funkcija localtime na osnovu broja sekundi kreira "razbijeni" oblik vremena
	 * Vreme se zapisuje u strukturi tipa tm. Struktura sadrzi polja koja
	 * opisuju dan, mesec, godinu, sate, minute, sekunde itd
	 * 
	 * BITNO:
	 * Pazljivo procitati u man stranama koji je format u kome se cuvaju ovi podaci
	 * 		npr. Meseci su u formatu 0-11, Dani u formatu 0-6 pri cemu je nulti dan nedelja
	 */ 
	struct tm* brokenTime = localtime(&now);
	check_error(brokenTime != NULL, "localtime");
	printf("Date: %d.%d.%d\n", brokenTime->tm_mday, brokenTime->tm_mon+1, brokenTime->tm_year + 1900);
	printf("%d:%d:%d\n", brokenTime->tm_hour, brokenTime->tm_min, brokenTime->tm_sec);

	exit(EXIT_SUCCESS);
}
