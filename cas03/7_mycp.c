/*
Napisati program koji kopira fajl sa jedne lokacije na drugu.

Poziv programa:
./os_cp destination_file source_file
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

/* funkcija iz prethodnog zadatka*/
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

/* funkcija kopira sadrzaj fajla sa fajl deskriptorom fdSrc u fajl sa fajl
 * deskriptorom fdDest
 */
bool os_copy_file_fd(const int fdDest, const int fdSrc) {
    /*
     * Alocira se bafer za citanje fajla
     */
    static const uint32_t memBufSize = 1U << 13; // 8KB
    char *memBuf = malloc(memBufSize);
	/* BITNO: obrada gresaka je neophodna prilikom svakog alociranja */
    if (NULL == memBuf)
        return false;
    
	/* promenljiva koja cuva broj ucitanih bajtova */
    int32_t readBytes;
	/* sve dok mozemo da ucitamo nesto iz fajla */
    while ((readBytes = read(fdSrc, memBuf, memBufSize)) > 0) {
		/* ono sto smo procitali sa jednog fd-a stampamo na drugi fd
		 *
		 * BITNO:
		 * uvek mozete da upisete u fajl najvise onoliko bajtova koliko ste procitali. Dakle, argument funkcije write
		 * moze da bude samo readBytes, nikako memBufSize
 		 */
        if(write(fdDest, memBuf, readBytes) < 0) {
            free(memBuf);
            return false;
        }
    }
	/* BITNO: sve sto se alocira, mora i da se oslobodi */
    free(memBuf);
	/* provera gresaka */
    return 0 == readBytes;
}

/* funkcija kopira sadrzaj fajla na putanji filePathSrc u fajl na putanji
 * filePathDest
 */
bool os_copy_file_path(const char *filePathDest, const char *filePathSrc) {
    /*
     * Deklarisemo promenljive za fajl deskriptore
     */
    int fdDest, fdSrc;
	/* otvaramo fajlove u odgovorajucim modovima i proveravamo greske */
    if (!os_file_open(filePathDest, "w", &fdDest) || !os_file_open(filePathSrc, "r", &fdSrc))
        return false;
    
    /*
     * sadrzaj fajla sa deskriptorom fdSrc se kopira u fajl da deskriptorom fdDest
     */
    bool result = os_copy_file_fd(fdDest, fdSrc);
	/* cuva se errno koji su postavili sistemski pozivi u funkciji osCopyFilePath */
    int saveErrno = errno;
	/* zatvaraju se deskriptori, sto za posledicu ima postavljanje errno na 0,
	 * tj. brisanje eventualnih gresaka koje su se desile u read i write sistemskim pozivima 
	 */
    close(fdDest);
    close(fdSrc);
    
    /*
     * Vracanje errno vrednosti koje su postavili read i write sistemski pozivi
     */
    errno = saveErrno;
	/* provera gresaka */
    return result;
}

int main(int argc, char **argv) {
    check_error(3 == argc, "Argument missing, usage: ./cpfile destination source");
    check_error(os_copy_file_path(argv[1], argv[2]), "File write to console");
    return 0;
}
