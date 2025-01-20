#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <errno.h>
#include <string.h>

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
	
	check_error(argc == 2, "./4 filePath word1 word2");
	
	FILE* f = fopen(argv[1], "r+");
	check_error(f != NULL, "fopen failed");
	
	int fd = fileno(f);
	check_error(fd != -1, "fileno failed");
	
	struct flock lock;
	char buf[MAX_SIZE];
	int len = strlen("Milivoje");
	
	while (fscanf(f, "%s", buf) == 1) {
		// ako nije Milivoje, prelazimo na sledecu rec
		if (strcmp(buf, "Milivoje")) {
			continue;
		}
		
		// pokusavamo da zakljucamo fajl u modu za pisanje
		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = ftell(f);
		lock.l_len = -len;
		
		// proveravamo da li je moguce zakljucati
		int result = fcntl(fd, F_GETLK, &lock);
		
		// provera gresaka
		if (result == -1) {
			if (errno != EACCES && errno != EAGAIN) {
				check_error(0, "lock failed");
			}
		}
		else {
			
			// ako postoje katanci, stampamo informacije o njima
			if (lock.l_type == F_RDLCK) {
				printf("r %ld %jd\n", ftell(f) - len, (intmax_t)lock.l_pid);
			}
			else if (lock.l_type == F_WRLCK) {
				printf("w %ld %jd\n", ftell(f) - len, (intmax_t)lock.l_pid);
			}
			else {
				// ako ne postoje katanci
								
				// zakljucavamo fajl
				check_error(fcntl(fd, F_SETLK, &lock) != -1, "fcntl failed");
				
				// pomeramo offset i menjamo Milivoja sa Dragutinom
				check_error(fseek(f, -len, SEEK_CUR) != -1, "fseek failed");
				fprintf(f, "%s", "Dragutin");
				
				/* otljucavanje fajla */
				lock.l_type = F_UNLCK;
				lock.l_whence = SEEK_SET;
				lock.l_start = 0;
				/* nula kao duzina je specijalni znak za zakljucavanje/otkljucavanje 
				 * fajla od pozicija start pa sve do kraja fajla
				 */
				lock.l_len = 0; 
				check_error(fcntl(fd, F_SETLK, &lock) != -1, "unlocking failed");
			}
		}
	}
	
	fclose(f);
		
	exit(EXIT_SUCCESS);
}
