/* Program obilazi direktorijum i racuna njegove velicinu
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
	
	/* ako je u pitanju dirketorijum, otvaramo ga */
	DIR* dir = opendir(putanja);
	check_error(dir != NULL, "...");
	
	/* u petlji citamo sadrzaj direktorijuma */
	struct dirent* dirEntry = NULL;
	errno = 0;
	while ((dirEntry = readdir(dir)) != NULL) {
		/* u svakom trenutku treba da budemo svesni svog trenutnog radnog direktorijuma (pwd)
		 * pwd nam je u svakom trenutku ona putanja koju nam je korisnik prosledio na pocetku programa,
		 * pa u svakom trenutku moramo rucno da pravimo putanje do svih drugih fajlova koje srecemo u
		 * obilasku
		 *
		 * na nasu tekucu putanju, moramo da dodamo "/" trenutno ima fajla iz direktorijuma koji smo otvorili
		 * i terminirajucu nulu 
		 */
		
		/* dinamicki alociramo prostor za novu putanju */
		char* path = malloc(strlen(putanja) + strlen(dirEntry->d_name) + 2);
		check_error(path != NULL, "...");
		
		/* formiramo putanju na gore opisani nacin */
		strcpy(path, putanja);
		strcat(path, "/");
		strcat(path, dirEntry->d_name);
		
		//sprintf(path, "%s/%s", putanja, dirEntry->d_name);
		
		/* u slucaju da se radi o "." ili ".." direktorijumima, moramo da ih preskocimo,
		 * jer u suprotnom upadamo u beskoacnu rekurziju
		 */ 
		if (!strcmp(dirEntry->d_name, ".") || !strcmp(dirEntry->d_name, "..")) {
			/* iako ih preskacemo, to ne znaci da treba da ih iskljucimo iz razmatranja */
			
			/* citamo informacije o fajlu */
			check_error(stat(path, &fInfo) != -1, "...");
			/*dodajemo velicinu fajla na tekuci zbir */
			*psize += fInfo.st_size;
			
			free(path);
			errno = 0;
			/* i prelazimo na sledeci fajl u direktorijumu */
			continue;
		}
		/* ako se ne radi o "." i ".." , rekurzivno pozivamo funkciju i obradjujemo fajl */
		sizeOfDir(path, psize);
		free(path);
		
		errno = 0;
	}
	
	check_error(errno != EBADF, "readdir");
	/* zatvaramo direktorijum */
	check_error(closedir(dir) != -1, "...");	
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
