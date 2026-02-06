#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <ftw.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

bool is_public_file(const char *fpath)
{
    struct stat fileinfo;
    check_error(stat(fpath, &fileinfo) != -1, "Nepostojeći fajl");

    /* proveravamo da li je prosledjen ispravan tip fajla */
    check_error(S_ISREG(fileinfo.st_mode), "Pogresan tip fajla");

    /* proveravamo da li ostali imaju trazena prava */
    if (fileinfo.st_mode & S_IROTH && fileinfo.st_mode & S_IWOTH)
        return true;

    return false;
}

void make_public_dir(const char *dname)
{
    mode_t mode = 0777;
    int k = mkdir(dname, mode);
    check_error(k != -1, "mkdir");

    /* zbog umask-a, mora se dodatno obezbediti da su tražena prava postavljena */
    /* drugi način je postavljanje umask-a na 0, tj. umask(0) */
    check_error(chmod(dname, mode) != -1, "chmod");
}

unsigned n_days_modified(const char *fpath)
{
    time_t now = time(NULL);
    struct stat fInfo;
    check_error(stat(fpath, &fInfo) != -1, "Ne postoji");

    /* broj sekundi od poslednje promene */
    time_t razlika = now - fInfo.st_mtim.tv_sec;

    return razlika / (60 * 60 * 24);
}

void move_file(const char *srcPath, const char *destPath)
{
    check_error(strcmp(srcPath, destPath) != 0, "isti fajl");

    struct stat fInfo;
    check_error(stat(srcPath, &fInfo) != -1, "stat");

    int fdSrc = open(srcPath, O_RDONLY);
    check_error(fdSrc != -1, "open1");

    int fdDest = open(destPath, O_WRONLY | O_CREAT | O_TRUNC);
    check_error(fdDest != -1, "open2");

    int bytesRead = 0;
    char memBuff[1024];
    while ((bytesRead = read(fdSrc, memBuff, 1024)) > 0) {
        check_error(write(fdDest, memBuff, bytesRead) != -1, "write");
    }
    check_error(bytesRead != -1, "write");

    close(fdSrc);
    close(fdDest);

    check_error(unlink(srcPath) != -1, "unlink");

    check_error(chmod(destPath, fInfo.st_mode) != -1, "chmod");
}

/* putanja do direktorijuma koji treba kreirati i u koji treba premestiti fajlove*/
char newDir[1024];

int process_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (S_ISREG(sb->st_mode)) {
        if (is_public_file(fpath)) {
            if (n_days_modified(fpath) < 30) {
                /* putanja na koju treba premestiti fajl*/
                char *destpath = malloc(strlen(newDir) + strlen(fpath + ftwbuf->base) + 2);
                strcpy(destpath, newDir);
                strcat(destpath, "/");
                strcat(destpath, fpath + ftwbuf->base);

                move_file(fpath, destpath);

                free(destpath);
            } else {
                check_error(unlink(fpath) != -1, "unlink");
            }
        }
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    check_error(argc == 3, "./a.out search_dir output_dir");
    struct stat fileinfo;
    check_error(stat(argv[1], &fileinfo) != -1, "ne postoji1");
    check_error(S_ISDIR(fileinfo.st_mode), "nije dir");

    strcpy(newDir, argv[2]);
    make_public_dir(argv[2]);

    check_error(nftw(argv[1], process_file, 50, 0) != -1, "nftw");

    return 0;
}