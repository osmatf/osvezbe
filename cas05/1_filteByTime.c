/* Program rekurzivno izdvaja sve fajlove koji su modifikovani 
 * u poslednjih n dana u direktorijumu na putanji koju prosledi korisnik
 *
 * Primer poziva:
 * ./filterByTime dirPath days
 */
#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <errno.h>
#include <ftw.h>

#include <string.h>
#include <time.h>

/* makro za proveru gresaka */
#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

#define DAY_IN_SEC (24*60*60)

int days = 0;

/* pokazivac na funkciju kojom se obradjuje svaki fajl na koji nftw naidje */
int filterByTime(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf) {
	
	/* citanje trenutnog vremena */
	time_t now = time(NULL);
	/* racunanje pre koliko dana je bila poslednja promena */
	time_t diffInSec = now - sb->st_mtime;
	
	/* ako je promena bila u poslednjih days dana */
	if (diffInSec/DAY_IN_SEC < days)
		/* ispisati ime fajla */
		printf("%-80s\n", fpath);
	
	/* povratna vrednost funkcije razlicita od 0 signalizira gresku i prekida rekurziju */
	return 0;
}

int main(int argc, char** argv) {
	
	/* provera broja argumenata */
	check_error(argc == 3, "./filterExt path days");
	
	/* prebacivanje stringa u broj */
	days = atoi(argv[2]);
	
	/* provera da li se radi o direktorijumu */
	struct stat fInfo;
	check_error(stat(argv[1], &fInfo) != -1, "stat failed");
	check_error(S_ISDIR(fInfo.st_mode), "not a dir");
	
	/* obilazak direktorijuma pomocu ugradjene funkcije */
	check_error(nftw(argv[1], filterByTime, 50, 0) != -1, "nftw failed");
	
	/* zavrsavamo program sa odgovarajucim statusom */
	exit(EXIT_SUCCESS);
}
