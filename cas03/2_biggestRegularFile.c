/*
Kao argumenti komandne linije prosledjuju se fajlovi (razlicitih tipova).
Ispisati velicinu najveceg regularnog fajla od prosledjenih.

Pretpostaviti da nijedan fajl nece biti simbolicki link i da ce biti
prosledjen barem jedan regularan fajl.

Poziv programa:
./a.out <file1> <file2> ... <filen>
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

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
    check_error(argc >= 2, "./a.out <file1> <file2> ... <filen>");

    long int max_size = 0;
    for (int i = 1; i < argc; i++) {
        struct stat fileinfo;
        check_error(stat(argv[i], &fileinfo) != -1, "stat");
        /*
         * Makro S_ISREG() se koristi za proveru da li dati fajl
         * predstavlja regularni fajl.
         *
         * Informacija o tipu fajla (regularni fajl, direktorijum,
         * simbolička veza, uređaj, itd.) kodirana je u polju
         * st_mode strukture stat.
         *
         * S_ISREG(m) proverava da li bitovi u m odgovaraju
         * tipu "regularni fajl".
         *
         * Analogno za ostale tipove (pogledati man 7 inode).
         */
        if (S_ISREG(fileinfo.st_mode) && fileinfo.st_size > max_size) {
            max_size = fileinfo.st_size;
        }
    }

    printf("Velicina najveceg prosledjenog regularnog fajla je %ld\n", max_size);

    return 0;
}
