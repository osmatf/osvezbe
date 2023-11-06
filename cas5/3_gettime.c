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
	
	/* funkcija gettimeofday() omogucava preciznije merenje vremena, tj.
	 * format vremena je sada u sekundama i mikrosekundama.
	 * 
	 * Vreme u ovom formatu se upisuje u strukturu tipa timeval koja ima
	 * dva polja:
	 * tv_usec -> mikrosekunde
	 * tv_sec -> sekunde
	 */ 
	struct timeval preciseTime;
	check_error(gettimeofday(&preciseTime, NULL) != -1, "lklk");
	
	/* provera koliko time gresi u odnosu na gettimeofday */
	if (now == preciseTime.tv_sec) {
		printf("time() == gettimeofday()\n");
	}
	else {
		printf("time() != gettimeofday()\n");
	}
	printf("Time diff: %d\n", (int)preciseTime.tv_usec);
	
	
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
	check_error(brokenTime != NULL, "...");
	printf("Date: %d.%d.%d\n", brokenTime->tm_mday, brokenTime->tm_mon+1, brokenTime->tm_year + 1900);
	printf("%d:%d:%d\n", brokenTime->tm_hour, brokenTime->tm_min, brokenTime->tm_sec);
	
	
	/* Manipulacija vremenom i datumima treba da se radi pomocu localtime uvecavanjem ili
	 * smanjivanjem odgovarajucih polja strukture tm. Nije ideja da se polomite komplikovanom
	 * matematikom u nadi da cete ubosti ispravno resenje. 
	 */
	 
	/* pomeramo vreme za godinu dana unapred */ 
	brokenTime->tm_year++;
	/* funkcija mktima omogucava dobijanje broja sekundi od epohe na osnovu
	 * "razbijenog" vremena
	 */ 
	time_t future = mktime(brokenTime);
	check_error(future != -1, "...");
	/* prikazujemo koliko iznosi godinu dana u sekundama */
	printf("year in sec: %d\n", (int)(future - now));
	
	/* uvecavamo broj dana za neku nelogicnu cifru
	 * Ovim uvecavanjem ispada da bi trebalo da imamo 40 i neki dan u mesecu.
	 * i da dobijemo nekorektno vreme
	 */ 
	brokenTime->tm_mday+=25;
	/* prebacivanjem tog vremena u sekunde */
	time_t newTime = mktime(brokenTime);
	check_error(newTime != -1, "...");
	/* i njegovim prikazivanjem, uvericemo se da nije bas tako */
	printf("%s\n", ctime(&newTime));
	
	exit(EXIT_SUCCESS);
}
