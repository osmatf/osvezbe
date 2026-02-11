#define _XOPEN_SOURCE 500

#include <ftw.h>
#include <grp.h>
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

int broj_clanova(gid_t gid)
{
    struct group *group_info = getgrgid(gid);
    check_error(group_info != NULL, "group");

    int i = 0;
    while (group_info->gr_mem[i] != NULL) {
        i++;
    }

    return i;
}

int obradi_fajl(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (!S_ISREG(sb->st_mode) && ftwbuf->level == 3 && broj_clanova(sb->st_gid) >= 1) {
        printf("%s\n", fpath);
    }

    return 0;
}

int main(int argc, char **argv)
{
    check_error(argc == 1, "./a.out");

    /* postavljamo potpuno baferisanje standardnog izlaza */
    check_error(setvbuf(stdout, NULL, _IOFBF, 0) == 0, "setvbuf");

    char *dir_path = getenv("PUTANJA");
    check_error(dir_path != NULL, "getenv");

    check_error(nftw(dir_path, obradi_fajl, 50, 0) != -1, "nftw");

    return 0;
}