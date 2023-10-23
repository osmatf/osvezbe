/*
Primer detaljnije obrade greske. Kodovi gresaka su definisani
unutar zaglavlja errno.h kao makroi. Kod greske sistemski pozivi
beleze u promenljivoj errno.
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>



int main(){
	/*
	Promenljiva koja je deklarisana unutar errno.h.
	Postaviti ovu vrednost na 0 pre poziva koji ce je
	potencijalno promeniti
	*/
	errno = 0;

	FILE *ulaz = fopen("ulaz.txt","r");

	if(errno != 0) { //doslo je do greske
		if (errno == ENOENT) {
			fprintf(stderr, "Ne postoji datoteka\n");
			exit(EXIT_FAILURE);
		} else if (errno == EACCES) {
			fprintf(stderr, "Nemamo prava pristupa\n");
			exit(EXIT_FAILURE);	
		} else {
			fprintf(stderr, "Neka druga greska\n");
			exit(EXIT_FAILURE);	
		}
	}

	return 0;
}