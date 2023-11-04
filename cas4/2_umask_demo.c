/* Program kreira fajl pravima pristupa koja zeli korisnik.
 * U slucaju da fajl postoji, bice mu promenjena prava pristupa.
 *
 * Primer poziva:
 * ./umask_demo test.txt 777
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <stdbool.h>
/* makro za proveru gresaka */
#define check_error(expr, userMsg) \
	do { \
		if (!(expr)) {\
			perror(userMsg); \
			exit(EXIT_FAILURE); \
		}\
	} while (0)
	
int main(int argc, char** argv) {
	
	/* provera broja argumenata */
	check_error(argc == 3, "./umask_demo path priviliges");
	
	/* da bi se kreirao fajl sa proizvoljnim pravima pristupa
	 * potrebno je postaviti umask na nula
	 *
	 * umask je vrednost kojom se odredjena prava pristupa automatski gase prilikom
	 * kreiranja fajla, tj. sluzi kao bezbednosna zastita od nepromisljenih poteza korisnika.
	 *
	 * Na sistemima umask najcesce ima vrednost 0022. Ova vrednost znaci da se grupi i ostalim
	 * korisnicima onemogucava izmena fajla.
	 *
	 * To se postize na sledeci naci: Recimo da su zeljena prava pristupa 0777 i da umask ima 
	 * vrednost 0022. Konacna prava pristupa (pp) koja ce imati fajl se dobijaju po sledecoj formuli
	 *
	 * pp = zelja & ~umask
	 *
	 * zelja (0777): 111 111 111
	 * umask (0022): 000 010 010
	 *
	 * pp = zelja & !umask
	 * 111 111 111		 zelja
	 * 111 101 101		~umask
	 * ------------   	 &
	 * 111 101 101		pp (0755)
	 *
	 * BITNO:
	 * umask sluzi kao mehanizam kojim se odredjena prava pristupa gase prilikom kreiranja fajlova.
	 *
	 * JOS BITNIJE:
	 * umask je beskoristan ako se radi o fajlovma koji vec postoje. Primenljiv je samo kada se
	 * kreiraju novi fajlovi. Promena prava pristupa postojecim fajlovima se radi pomocu chmod sist poziva.
	 */
	 
	/* postavljamo umask na nula, tj. korisniku je sve dozvoljeno */
	mode_t oldUmask = umask(0);
	
	/* pretvaramo drugi atgument komandne linije iz stringa u broj sa osnovom 8 */
	long mode = strtol(argv[2], NULL, 8);
	
	/* pokusavamo da kreiramo fajl sa pravima pristupa koje zeli korisnik
	 *
	 * O_EXCL flag nam omogucava da otkrijemo ako fajl vec postoji, tj. puci ce 
	 * open poziv pri cemu ce postaviti errno na EEXIST
	 */
	int fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, mode);
	/* ispitujemo da li je pukao open poziv */
	if (fd == -1) {
		/* ako je uzrok pucanja fajla njegovo postojanje */
		if (errno == EEXIST) {
			
			printf("Fajl vec postoji\n");
			
			/* prava pristupa menjamo pomocu funkcije chmod */
			check_error(chmod(argv[1], mode) != -1, "chmod failed");
		}
		/* ako je bilo sta drugo uzrok pucanja open poziva, prekidamo izvrsavanje */
		else {
			
			check_error(false, "file open failed");
		}
	}
	/* ako smo otvorili fajl */
	else {
		
		/* potrebno je da ga zatvorimo */
		close(fd);
	}
	
	/* na kraju vracamo staru vrednost umaska */
	umask(oldUmask);
	
	/* zavrsavamo program sa odgovarajucim statusom */
	exit(EXIT_SUCCESS);
}
