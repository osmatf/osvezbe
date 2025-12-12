/*
Ispisati informacije o grupi ciji se naziv zadaje kao
argument komandne linije.

Poziv programa:
./groupInfo <group_name>
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <grp.h>

#define check_error(cond, userMsg)                                                                 \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            fprintf(stderr, "Greska: %s\n", userMsg);                                              \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

int main(int argc, char **argv)
{
    check_error(argc == 2, "./a.out <group_name>");

    struct group *groupInfo = getgrnam(argv[1]);
    check_error(groupInfo != NULL, "getgrnam");

    printf("Group name: %s\n", groupInfo->gr_name);
    printf("Group password: %s\n", groupInfo->gr_passwd);
    printf("Group ID: %jd\n", (intmax_t)groupInfo->gr_gid);
    for (int i = 0; groupInfo->gr_mem[i] != NULL; i++) {

        printf("\t%s\n", groupInfo->gr_mem[i]);
    }

    exit(EXIT_SUCCESS);
}
