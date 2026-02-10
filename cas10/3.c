#define _XOPEN_SOURCE 500
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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
    check_error(argc == 1, "./a.out");

    DIR *dir = opendir(".");
    check_error(dir != NULL, "opendir");

    struct dirent *dir_entry = NULL;
    errno = 0;
    while ((dir_entry = readdir(dir)) != NULL) {
        if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0) {
            errno = 0;
            continue;
        }

        struct stat fileinfo;
        check_error(lstat(dir_entry->d_name, &fileinfo) != -1, "lstat");

        if (S_ISDIR(fileinfo.st_mode)) {

            errno = 0;
            rmdir(dir_entry->d_name);
            /* u slucaju nepraznog direktorijuma ne zelimo da program pukne,
             * vec nastavljamo sa obilaskom
             */
            if (errno != 0 && errno != ENOTEMPTY) {
                /* u svim drugim slucajevima kada je errno != 0 u pitanju je greska */
                check_error(0, "rmdir");
            }
        }

        errno = 0;
    }
    check_error(errno == 0, "readdir");

    closedir(dir);

    return 0;
}