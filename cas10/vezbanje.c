/*
Napisati program koji preko promenljive okruzenja PUTANJA dobija putanju do fajla. Program treba da čita
fajl rec po rec i prebroji reci koje nisu zakljucane. Rec nece biti duza od 128 karaktera.
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

#define MAX_SIZE (128)

int main(int argc, char** argv) {
    check_error(argc == 1, "./a.out");

    char *putanja = getenv("PUTANJA");
    check_error(putanja != NULL, "promenljiva okruzenja PUTANJA nije postavljena");

    int fd = open(putanja, O_RDWR);
    check_error(fd != -1, "open");

    FILE* f = fdopen(fd, "r+");
    check_error(f != NULL, "fdopen");

    char buffer[MAX_SIZE];
    int num = 0;
    //citamo rec po rec u fajlu
    while (fscanf(f, "%s", buffer) == 1) {
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
            //povecavamo broj
            num++;
            //otkljucavamo zakljucanu rec
            lock.l_type = F_UNLCK;
            check_error(fcntl(fd, F_SETLK, &lock) != -1, "unlocking failed");
        }
        else {
            if (errno != EACCES && errno != EAGAIN) {
                check_error(0, "locking failed");
            }
            //ukoliko nije postavljen katanac jer neko drugi ima katanac, idemo dalje
            continue;
        }
    }

    printf("%d\n", num);

    fclose(f);

    exit(EXIT_SUCCESS);
}
