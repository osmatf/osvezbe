/*
Program brise sadrzaj fajla, a zatim postavlja vreme pristupa i 
modifikacije fajla na vreme pre brisanja sadrzaja.

Poziv programa:
./truncateUndetected filePath
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* neophodno zaglavlje za upotrebu funkcije utime(...) */
#include <utime.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

static char* osUsage = "./truncateUndetected filePath";

/* funkcija brise sadrzaj fajla i uklana informaciju o pristupu */
void os_truncate(const char* filePath) {
	
	/* uz pomoc stata citamo informacije o fajlu */
	struct stat fInfo;
	check_error(stat(filePath, &fInfo) != -1, "stat failed");
	
	/* brisemo sadrzaj fajla tako sto ga otvorimo u modu za pisanje */
	FILE* f = fopen(filePath, "w");
	check_error(f != NULL, "fopen failed");
	fclose(f);
	
	/* upisujemo podatke o prethodnom vremenu pristupa i modifikacije
	 * u strukturu utimbuf
	 */ 
	struct utimbuf acTime;
	acTime.actime = fInfo.st_atime;
	acTime.modtime = fInfo.st_mtime;
	
	/* uz pomoc funkcije utime(...) menjamo vreme pristupa i 
	 * modifikacije fajla 
	 */ 
	check_error(utime(filePath, &acTime) != -1, "utime failed");
}

int main(int argc, char** argv) {
	
	check_error(argc == 2, osUsage);
	
	os_truncate(argv[1]);
	
	exit(EXIT_SUCCESS);
}
