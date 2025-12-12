/*
Ispisati nazive svih grupa koje imaju vise od n clanova.
Broj n se zadaje kao argument komandne linije.

Poziv programa:
./a.out n
*/
#define _XOPEN_SOURCE 700
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

int number_of_members(const struct group *group_info)
{
    int i = 0;
    while (group_info->gr_mem[i] != NULL) {
        i++;
    }

    return i;
}

int main(int argc, char **argv)
{
    check_error(argc == 2, "./a.out n");

    int n = atoi(argv[1]);

    setgrent();

    struct group *current_group = NULL;

    while ((current_group = getgrent()) != NULL) {

        if (number_of_members(current_group) > n) {
            printf("%s\n", current_group->gr_name);
        }
    }

    endgrent();

    exit(EXIT_SUCCESS);
}
