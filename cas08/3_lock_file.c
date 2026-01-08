/*
Napisati program koji kao argument komandne linije dobija putanju do regularnog fajla. Program treba da čita
fajl liniju po liniju. Ukoliko linija nije zaključana, program treba da je upiše u fajl čija se putanja zadaje kao drugi
argument komandne linije. Na kraju rada program treba da ispiše ukupan broj linija koje su upisane. Linija neće
biti duža od 255 karaktera.
*/
#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <ftw.h>

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

    check_error(argc == 3, "...");
    //otvaramo fajl za citanje i pisanje
    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "...");

    FILE* f = fdopen(fd, "r+");
    check_error(f != NULL, "...");

    FILE *output = fopen(argv[2], "w");
    check_error(output != NULL, "fopen");

    char buffer[MAX_SIZE];
    int numLines = 0;
    //citamo liniju po liniju u fajlu
    while (fgets(buffer, MAX_SIZE, f) != NULL) {
        //struktura koja opisuje katanac
        struct flock lock;
        lock.l_type = F_WRLCK;
        lock.l_start = ftell(f);
        lock.l_len = -strlen(buffer);
        lock.l_whence = SEEK_SET;
        //postavljamo katanac
        int retVal = fcntl(fd, F_SETLK, &lock);

        //ukoliko je uspesno postavljen katanac
        if (retVal != -1) {
            fprintf(output, "%s", buffer);
            //povecavamo broj linija
            numLines++;
            //otkljucavamo zakljucanu liniju
            lock.l_type = F_UNLCK;
            check_error(fcntl(fd, F_SETLK, &lock) != -1, "....");
        }
        else {
            if (errno != EACCES && errno != EAGAIN) {
                
                check_error(0, "...");
            }
            //ukoliko nije postavljen katanac jer neko drugi ima katanac, idemo dalje
            continue;
        }
    }

    printf("%d\n", numLines);

    fclose(f);
    fclose(output);

    exit(EXIT_SUCCESS);
}
