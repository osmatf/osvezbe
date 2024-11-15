/*
Napisati program koji cita n bajtova sa kraja fajla cija se putanja zadaje
kao argument komandne linije. Broj n se takodje zadaje kao argument komandne linije.
Procitani sadrzaj ispisati na standardni izlaz.

NAPOMENA: uraditi zadatak bez citanja pocetnog dela fajla (sadrzaj koji prethodi n-tom bajtu)

Poziv programa:
./read_bytes filepath n
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

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
    check_error(3 == argc, "Usage: ./file_open filename n");

    int n = atoi(argv[2]);

    /* otvaramo fajl za citanje */
    int fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open");

    /*
     * Funkcija lseek pomera offset fajla koji predstavlja
     * poziciju u fajlu sa koje ce se desiti naredna
     * IO operacija (read/write). Inicijalno je offset = 0.
	 *
	 * TLPI za detalje
     * 
     * Isto se moglo postici sa lseek(fd, filesize - n, SEEK_SET)
     */
    check_error(lseek(fd, -n, SEEK_END) != -1, "lseek");

    char *buffer = malloc(n + 1);
    check_error(buffer != NULL, "malloc");

    int procitano = read(fd, buffer, n);
    check_error(procitano != -1, "read");

    buffer[procitano] = '\0';
    printf("%s\n", buffer);

    free(buffer);
    close(fd);

    return 0;
}

