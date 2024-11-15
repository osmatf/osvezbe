/* Program obilazi direktorijum i racuna njegovu velicinu,
 * bez pravljenja pomocnih putanja
 *
 * Primer poziva:
 * ./sizeOfDir dirPath
 */
#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <dirent.h>

#include <string.h>

#define check_error(cond,userMsg)\
	do {\
		if (!(cond)) {\
			perror(userMsg);\
			exit(EXIT_FAILURE); \
		}\
	} while(0)


void sizeOfDir(char* putanja, unsigned* psize) {
	/* citamo informacije o trenutnom fajlu */
	struct stat fInfo;
	check_error(lstat(putanja, &fInfo) != -1, "...");
	/* dodajemo velicinu fajla na tekuci zbir */
	*psize += fInfo.st_size;
	
	/* ukoliko se ne radi o direktorijumu */
	if (!S_ISDIR(fInfo.st_mode)) {
		/* prekidamo rekurziju */
		return;
	}
	
	char* dirPath = realpath(putanja, NULL);
	printf("Dir: %s\n", dirPath);
	free(dirPath);
	
	/* ako je u pitanju dirketorijum, otvaramo ga */
	DIR* dir = opendir(putanja);
	check_error(dir != NULL, "...");
	
	/* da ne bismo rucno pravili putanje, mozemo da promenimo
	 * pwd i da udjemo u direktorijum
	 */
	check_error(chdir(putanja) != -1 , "...");
	
	/* u petlji citamo sadrzaj direktorijuma */
	struct dirent* dirEntry = NULL;
	errno = 0;
	while ((dirEntry = readdir(dir)) != NULL) {
		
		char* fajl = realpath(dirEntry->d_name, NULL);
		printf("\tFajl: %s: %s\n", fajl, dirEntry->d_name);
		
		free(fajl);
		/* u slucaju da se radi o "." ili ".." direktorijumima, moramo da ih preskocimo,
		 * jer u suprotnom upadamo u beskoacnu rekurziju
		 */ 
		if (!strcmp(dirEntry->d_name, ".") || !strcmp(dirEntry->d_name, "..")) {
			/* iako ih preskacemo, to ne znaci da treba da ih iskljucimo iz razmatranja */
			
			/* citamo informacije o fajlu */
			check_error(stat(dirEntry->d_name, &fInfo) != -1, "...");
			
			/*dodajemo velicinu fajla na tekuci zbir */
			*psize += fInfo.st_size;
			
			errno = 0;
			/* i prelazimo na sledeci fajl u direktorijumu */
			continue;
		}
		/* ako se ne radi o "." i ".." , rekurzivno pozivamo funkciju i obradjujemo fajl */
		sizeOfDir(dirEntry->d_name, psize);
		
		errno = 0;
	}
	
	check_error(errno != EBADF, "...");
	/* napustamo direktorijum koji smo obisli */
	check_error(chdir("..") != -1, "...");
	/* zatvaramo direktorijum */
	check_error(closedir(dir) != -1, "closedir");
}

int main(int argc, char** argv) {
	
	check_error(argc == 2, "...");
	
	struct stat fInfo;
	check_error(stat(argv[1], &fInfo) != -1, "...");
	//(fInfo.st_mode & S_IFMT) == S_IFDIR
	check_error(S_ISDIR(fInfo.st_mode), "nije dir");
	
	unsigned size = 0;
	sizeOfDir(argv[1], &size);
	
	printf("Dir size: %uB\n", size);
	
	exit(EXIT_SUCCESS);
}
