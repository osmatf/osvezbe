/* Program ilustruje formatirani prikaz vremena 

Napomena: nije neophodno za ispit, ali može biti korisno
*/
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

#define MAX_SIZE (1024)
int main(int argc, char** argv) {
	
	/* citamo trenutno vreme */
	time_t now = time(NULL);
	/* kreiramo "razbijeni" prikaz vremena */
	struct tm* brokenTime = localtime(&now);
	check_error(brokenTime != NULL, "...");
	
	char buffer[MAX_SIZE];	
	/* Dosadasnji prikaz vremena pomocu ctime je bio krajnje nefleksibilan.
	 * Funkcija strftime omogucava proizvoljno formatiranje vremena koje
	 * odgovara korisnikovim potrebama
	 * 
	 * BITNO:
	 * ako pogledate man strane videcete da strftime ima gomilu specifikatora
	 * konverzije. nije ideja da ih znate sve, vec treba samo da znate gde se nalaze
	 * i kako da nadjete bas ono sto vam treba
	 */ 
	int retVal = strftime(buffer, MAX_SIZE, "Century: %C\nISO: %F\n", brokenTime);
	check_error(retVal != -1, "...");
	printf("%s\n", buffer);
	
	exit(EXIT_SUCCESS);
}
