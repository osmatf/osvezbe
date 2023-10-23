/*
Program ispisuje informacije o fajlu po uzoru na 'ls -al'.

Poziv programa:
./fileinfo filePath
*/
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* zaglavlje neophodno za upotrebu errno promenljive */
#include <errno.h>
/* zaglavlje neophodno za upotrebu funkcija za citanje passwd fajla */
#include <pwd.h>
/* zaglavlje neophodno za upotrebu funkcija za citanje group fajle */
#include <grp.h>
/* zaglavlje neophodno za upotrebu funkcija za ctime(...) */
#include <time.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

#define MAX_SIZE 1024

static char* osUsage = "./fileInfo filePath";

/* funkcija cita korisnikovo ime iz passwd fajla
 * na osnovu numerickog id-a korisnika
 */
char* os_get_user_info_uid(uid_t uid) {
	
	/* citamo informacije iz fajla na osnovu numerickog id-a
	 * korisnika
	 */
	struct passwd* userInfo = getpwuid(uid);
	/* proveravamo greske */
	if (userInfo == NULL)
		return NULL;
	
	/* alociramo string za upis korisnickog imena
	 * 
	 * BITNO:
	 * struktura passwd je staticki alocirana, pa bi uvek trebalo
	 * da prekopiramo sadrzaj strukture u lokalni adresni prostor
	 */
	char* userName = malloc(strlen(userInfo->pw_name) + 1);
	if (userName == NULL)
		return NULL;
		
	/* koripamo korisnicko ime u lokalni adresni prostor */
	strcpy(userName, userInfo->pw_name);
	
	/* vracamo adresu alociranog stringa
	 * BITNO:
	 * string je dinamicki alociran, pa moramo da ga oslobodimo
	 * u pozivajucoj funkciji 
	 */
	return userName;
}

/* funkcija cita ime grupe iz group fajla
 * na osnovu numerickog id-a grupe
 */
char* os_get_group_info_gid(gid_t gid) {
	
	/* citamo informacije iz fajla na osnovu numerickog id-a
	 * grupe
	 */
	struct group* groupInfo = getgrgid(gid);
	/* proveravamo greske */
	if (groupInfo == NULL)
		return NULL;
	
	/* alociramo string za upis imena grupe
	 * 
	 * BITNO:
	 * struktura group je staticki alocirana, pa bi uvek trebalo
	 * da prekopiramo sadrzaj strukture u lokalni adresni prostor
	 */
	char* groupName = malloc(strlen(groupInfo->gr_name) + 1);
	if (groupName == NULL)
		return NULL;
	
	/* kopiramo ime grupe u lokalni adresni prostor */
	strcpy(groupName, groupInfo->gr_name);
	
	/* vracamo adresu alociranog stringa
	 * BITNO:
	 * string je dinamicki alociran, pa moramo da ga oslobodimo
	 * u pozivajucoj funkciji 
	 */
	return groupName;
}

/* funkcija kreira string reprezentaciju informacija o fajlu 
 * BITNO:
 * drugi argument funkcije je pokazivac na pokazivac da bismo mogli
 * da upisemo adresu dinamicki alociranog bloka memorije na heap-u
 * 
 * upotreba obicnog pokazivaca je pogresna, jer bismo adresu upisali u 
 * lokalnu kopiju vrednosti iz main-a i brisanjem stek okvira funkcije
 * bismo izgubili adresu alociranog bloka, tj. doslo bi do curenja memorije
 */
void os_file_info(const char* filePath, char** fileInfoStr) {
	
	/* postavljamo errno na Invalid argument */
	errno = EINVAL;
	check_error(*fileInfoStr == NULL, "String must be NULL initialized");
	/* ako je sve ok, vracamo errno na podrazumevanu vrednost */
	errno = 0;
	
	/* alociramo blok i derefenciranjem pokazivaca upisujemo 
	 * adresu alociranog bloka direktno u promenljivu u mainu
	 */ 
	*fileInfoStr = malloc(MAX_SIZE);
	check_error(*fileInfoStr != NULL, "alllocation failed");
	/* ako je sve ok, pravimo lokalnu kopiju pokazivaca na alocirani
	 * blok
	 */ 
	char* fileStr = *fileInfoStr;
	
	/* informacije o fajlu se dobijaju upotrebo funkcije stat
	 * man 2 stat
	 * 
	 * TLPI za detalje
	 */ 
	struct stat fInfo;
	check_error(stat(filePath, &fInfo) != -1, "stat failed");
	
	/* inicijalno podrazumevamo da nad fajlom postoje sva prava
	 * pristupa
	 */ 
	char* filePrivileges = "-rwxrwxrwx ";
	
	/* kopiramo privilegije u dinamicki alocirani string na heap-u */
	strcpy(fileStr, filePrivileges);
	
	/* otkrivamo koji je tip fajla u pitanju 
	 * 
	 * BITNO:
	 * voditi racuna o makroima S_IFREG i S_ISREG(fmt) i tome sta koji
	 * od njih predstavlja
	 * 
	 * man 2 stat i TLPI za detalje
	 */ 
	switch (fInfo.st_mode & S_IFMT) {
		
		case S_IFSOCK:
			fileStr[0] = 's';
			break;
		case S_IFLNK:
			fileStr[0] = 'l';
			break;
		case S_IFREG:
			fileStr[0] = '-';
			break;
		case S_IFBLK:
			fileStr[0] = 'b';
			break;
		case S_IFCHR:
			fileStr[0] = 'c';
			break;
		case S_IFDIR:
			fileStr[0] = 'd';
			break;
		case S_IFIFO:
			fileStr[0] = 'f';
			break;
	}
	
	
	/* otkrivamo koje privilegije poseduje fajl */
	if (!(fInfo.st_mode & S_IRUSR))
		fileStr[1] = '-';
	if (!(fInfo.st_mode & S_IWUSR))
		fileStr[2] = '-';
	if (!(fInfo.st_mode & S_IXUSR))
		fileStr[3] = '-';
	
	if (!(fInfo.st_mode & S_IRGRP))
		fileStr[4] = '-';
	if (!(fInfo.st_mode & S_IWGRP))
		fileStr[5] = '-';
	if (!(fInfo.st_mode & S_IXGRP))
		fileStr[6] = '-';
	
	if (!(fInfo.st_mode & S_IROTH))
		fileStr[7] = '-';
	if (!(fInfo.st_mode & S_IWOTH))
		fileStr[8] = '-';
	if (!(fInfo.st_mode & S_IXOTH))
		fileStr[9] = '-';
		
	/* trenutni broj upisanih karaktera u dinamicki alociranom stringu */
	unsigned offset = 11;
	
	/* koristimo sprintf za formatirani upis u string
	 * prilikom svakog upisa pomeramo pokazivac za offset bajtova da 
	 * ne bismo stalno prepisivali preko pocetka
	 *
	 * sprintf vraca ukupan broj upisanih bajtova. tim brojem moramo da 
	 * uvecamo offset da bismo mogli da se praivlno pozicioniramo za 
	 * naredni upis
	 */
	offset += sprintf(fileStr + offset, "%ju ", (uintmax_t)fInfo.st_nlink);
	
	/* iz passwd fajla citamo ime korisnika na osnovu njegovog
	 * numerickog id-a
	 */ 
	char* userName = os_get_user_info_uid(fInfo.st_uid);
	check_error(userName != NULL, "User info error");
	offset += sprintf(fileStr + offset, "%s ", userName);
	/* BITNO: 
	 * nakon upotrebe treba da obrisemo dinamicki 
	 * alocirani string
	 */ 
	free(userName);
	
	/* iz group fajla citamo ime grupe na osnovu njenog
	 * numerickog id-a
	 */
	char *groupName = os_get_group_info_gid(fInfo.st_gid);
	check_error(groupName != NULL, "Group info failed");
	offset += sprintf(fileStr + offset, "%s ", groupName);
	/* BITNO: 
	 * nakon upotrebe treba da obrisemo dinamicki 
	 * alocirani string
	 */
	free(groupName);
	
	/* stampamo velicinu fajla */
	offset += sprintf(fileStr + offset, "%jd ", (intmax_t)fInfo.st_size);
	
	/* stampamo vreme poslednje modifikacije fajla */
	time_t seconds = fInfo.st_mtime;
	/* da bismo dobili vreme u citljivom formatu koristimo 
	 * funkciju ctime(...)
	 */
	offset += sprintf(fileStr + offset, "%s ", ctime(&seconds));
	
	/* stampamo putanju do fajla */
	offset += sprintf(fileStr + offset - 2, " %s", filePath);
}

int main(int argc, char** argv) {
	
	check_error(argc == 2, osUsage);
	/* string inicijalizujemo sa NULL,
	 * tj. eksplicitno ga obelezavamo kao neinicijalizovanog
	 */ 
	char* fileInfoStr = NULL;
	/* kreiramo string reprezentaciju informacija o fajlu */
	os_file_info(argv[1], &fileInfoStr);
	/* stampamo string */
	printf("%s\n", fileInfoStr);
	/* BITNO:
	 * sve sto je dinamicki alocirano, mora da bude oslobodjeno
	 * u suprotnom imamo curenje memorije u programu
	 */ 
	free(fileInfoStr);
	
	exit(EXIT_SUCCESS);
}
