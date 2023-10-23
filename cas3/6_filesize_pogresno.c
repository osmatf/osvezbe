/*
Napisati program koji ispisuje velicinu fajla koriscenjem
sistemskog poziva lseek.

Napomena:
- ovo je pogresan nacin za izracunavanje velicine fajla.
Da bi program radio, neophodno je pravo citanja nad zadatim fajlom,
sto ne bi trebalo da bude slucaj, jer zapravo ne citamo sadrzaj fajla.
Ovaj problem se prevazilazi koriscenjem sistemskog poziva stat
koji cemo raditi na sledecem casu.

Poziv programa:
./filesize filename

Domaci:
Umesto sa regularnim fajlom, probati sa standardnim izlazom (STDOUT_FILENO).
lseek ce puci. Zasto?
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

bool os_file_open(const char *filePath, const char *mode, int *fd) {
    /*
     * Privilegije se postavljaju na rw-r--r-- (0644)
     */
    static mode_t defaultMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    
    /*
	 * Postupno se dodaju flagovi za open funkciju
     */
    int flags = 0;
    switch (mode[0]) {
    case 'r':
		/* u slucaju citanja ukljucuje se:
		 * O_RDONLY - samo citanje
		 * O_RDWR - kombinovano citanje i pisanje, fajl offset na pocetku, ako se ne pise po fajlu
		 * 			sadrzaj ce biti nepromenjen
		 */
        flags |= '+' == mode[1] ? O_RDWR : O_RDONLY;
        break;
    case 'w':
		/* u slucaju citanja ukljucuje se:
		 * O_WRONLY - samo pisanje
		 * O_RDWR - kombinovano citanje i pisanje, fajl offset na pocetku, sadrzaj fajla se brise
		 */
        flags |= '+' == mode[1] ? O_RDWR : O_WRONLY;
        flags |= O_TRUNC;	/* flag kojim se brise sadrzaj fajla */
        flags |= O_CREAT;	/* flag kojim se fajl kreira u slucaju da ne postoji */
        break;
    case 'a':
		/* u nadovezivanja citanja ukljucuje se:
		 * O_WRONLY - samo pisanje
		 * O_RDWR - kombinovano citanje i pisanje, fajl offset na kraju
		 */
        flags |= '+' == mode[1] ? O_RDWR : O_WRONLY;
        flags |= O_APPEND;		/* flag kojim se ukljucuje pisanje ISKLJUCIVO na kraj fajla */
        flags |= O_CREAT;		/* flag kojim se fajl kreira u slucaju da ne postoji */
        break;
    default:
        return false;
    }
    
    /*
     * Open sistemskim pozivom se otvara fajl 
	 *
	 * TLPI za detalje
     */
    *fd = open(filePath, flags, defaultMode);
	/* provera gresaka */
    return *fd >= 0;
}

int main(int argc, char **argv) {
    check_error(2 == argc, "Argument missing");
    
    /*
     * Otvaramo fajl u modu za citanje 
     */
    int fd;
    check_error(os_file_open(argv[1], "r", &fd), "File open");
    
    /*
     * Funkcija lseek pomera offset fajla. Pomeranjem offseta na kraj,
	 * kao posledicu dobijamo velicinu fajla
	 *
	 * TLPI za detalje
     */
    off_t fileSize = lseek(fd, 0, SEEK_END);
    check_error(fileSize >= 0, "Lseek failed");

    close (fd);
    
    printf("%jd\n", (intmax_t)fileSize);
    
    return 0;
}
