/*
Program kreira 20 fajlova i upisuje 2 000 000 brojeva u svaki od njih.
Ovaj primer sluzi da demonstrira razliku u brzini izvrsavanja programa
bez i sa koriscenjem niti. Ovaj program ne koristi niti.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

#define NUM_OF_FILES 20
#define NUM_OF_NUMBERS 2000000

/* Funkcija otvara fajl i upisuje 2 000 000 brojeva u otvoreni fajl*/
void write_numbers_to_file(const char *filename)
{
    FILE *f = fopen(filename, "w");
    check_error(f != NULL, "fopen");

    for (int i = 0; i < NUM_OF_NUMBERS; i++) {
        fprintf(f, "%d ", i);
    }

    fclose(f);

    return;
}

int main(int argc, char **argv)
{
    check_error(argc == 1, "args");

    printf("Writing numbers...\n");
    for (int i = 0; i < NUM_OF_FILES; i++) {
        char filename[10];
        sprintf(filename, "%d.txt", i);

        write_numbers_to_file(filename);
    }
    printf("Done!\n");

    printf("Deleting files...\n");
    for (int i = 0; i < NUM_OF_FILES; i++) {
        char filename[10];
        sprintf(filename, "%d.txt", i);

        check_error(unlink(filename) != -1, "unlink");
    }
    printf("Done!\n");

    return 0;
}