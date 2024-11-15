/*
Napisati program koji zadati string upisuje u fajl koriscenjem sistemskog poziva write.
Putanja do fajla kao i zadati string se zadaju kao argumenti komandne linije.
Ukoliko zadati fajl ne postoji kreirati ga, a ukoliko postoji pre pisanja mu obrisati sadrzaj.

Poziv programa:
./write_bytes filepath some_text
./write_bytes 1.txt "ovo je neki tekst"
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

int main(int argc, char **argv) {
	/* provera broja argumenata */
    check_error(3 == argc, "Usage: ./write_bytes filepath some_text");

    /*
     * Privilegije se postavljaju na rw-r--r-- (0644)
     */
    mode_t default_mode = 0644;

    /* otvaramo fajl samo za pisanje (O_WRONLY). Dodatno
     * hocemo da se fajl kreira ukoliko ne postoji (O_CREAT) i
     * da mu se obrise sadrzaj ukoliko fajl postoji (O_TRUNC).
     * 
     * Kada postoji mogucnost da se fajl kreira (O_CREAT) onda je
     * neophodno zadati i treci argument pozivu open koji predstavlja
     * prava pristupa koja ce novokreirani fajl imati.
     */
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, default_mode);
    check_error(fd != -1, "open");

    /* NAPOMENA: terminirajucu nulu nikad ne upisujemo u fajl, 
     * ona samo sluzi programu da zna gde smo zamislili da se string
     * zavrsi.
     */
    int broj_bajtova_za_upis = strlen(argv[2]);

    /*
     * Sistemski poziv write se koristi za upisivanje bajtova u fajl.
	 *
	 * TLPI za detalje
     */
    int upisano = write(fd, argv[2], broj_bajtova_za_upis);
    check_error(upisano == broj_bajtova_za_upis, "write");

    close(fd);

    return 0;
}

