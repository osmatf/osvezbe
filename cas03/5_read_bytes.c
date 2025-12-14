/*
Napisati program koji cita n bajtova iz fajla cija se putanja zadaje
kao argument komandne linije. Broj n se takodje zadaje kao argument komandne linije.
Procitani sadrzaj ispisati na standardni izlaz.

Poziv programa:
./read_bytes filepath n
*/
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

int main(int argc, char **argv)
{

    /* provera broja argumenata */
    check_error(3 == argc, "Usage: ./file_open filename n");

    int n = atoi(argv[2]);
    /*
     * Open sistemskim pozivom se otvara fajl. Flegom O_RDONLY
     * odredjujemo da cemo samo citati iz fajla.
     *
     * TLPI za detalje
     */
    int fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open");

    /*
     * Sistemski poziv read se koristi za citanje bajtova iz fajla
     * (podsetimo se, fajlovi su zapisani kao niz bajtova). Da bi smo
     * mogli da citamo obezbedjujemo gde ce bajtovi biti smesteni, tj.
     * alociramo buffer velicine n + 1 (zbog terminirajuce nule).
     *
     * TLPI za detalje
     */
    char *buffer = malloc(n + 1);
    check_error(buffer != NULL, "malloc");

    /*NAPOMENA: read moze da procita manje bajtova nego sto je trazeno,
     * recimo ako je fajl manji od n.
     */
    int procitano = read(fd, buffer, n);
    check_error(procitano != -1, "read");

    /*BITNO: read nikada ne stavlja terminirajucu nulu na kraj stringa,
     * vec iskljucivo radi sa bajtovima (nema ideju o konceptu "string").
     * Zato mi moramo staviti terminirajucu nulu nakon poslednjeg ucitanog bajta.
     */
    buffer[procitano] = '\0';
    printf("%s\n", buffer);

    free(buffer);

    /* BITNO: svaki otvoreni fajl mora da se zatvori */
    close(fd);

    return 0;
}
