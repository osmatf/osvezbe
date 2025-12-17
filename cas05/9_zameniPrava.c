/*
Poziv programa:
./a.out pathToFile

Program treba da zameni prava pristupa koje ima grupa
sa pravima pristupa koje imaju ostali.
('man 2 stat' i 'man 7 inode' sadrze sve sto je potrebno)
*/

#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <time.h>

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

    check_error(argc == 2, "...");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat");
    /*
    mode_t pravaPristupa = fInfo.st_mode & S_IRWXU;
    pravaPristupa |= (fInfo.st_mode & S_IRWXG) >> 3;
    pravaPristupa |= (fInfo.st_mode & S_IRWXO) << 3;
    */

    mode_t pravaPristupa = 0;

    if (fInfo.st_mode & S_IRUSR)
        pravaPristupa |= S_IRUSR;
    if (fInfo.st_mode & S_IWUSR)
        pravaPristupa |= S_IWUSR;
    if (fInfo.st_mode & S_IXUSR)
        pravaPristupa |= S_IXUSR;

    if (fInfo.st_mode & S_IRGRP)
        pravaPristupa |= S_IROTH;
    if (fInfo.st_mode & S_IWGRP)
        pravaPristupa |= S_IWOTH;
    if (fInfo.st_mode & S_IXGRP)
        pravaPristupa |= S_IXOTH;

    if (fInfo.st_mode & S_IROTH)
        pravaPristupa |= S_IRGRP;
    if (fInfo.st_mode & S_IWOTH)
        pravaPristupa |= S_IWGRP;
    if (fInfo.st_mode & S_IXOTH)
        pravaPristupa |= S_IXGRP;

    check_error(chmod(argv[1], pravaPristupa) != -1, "chmod");

    exit(EXIT_SUCCESS);
}
