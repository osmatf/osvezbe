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
    check_error(argc == 2, "./a.out <group_name>");

    struct group *groupInfo = getgrnam(argv[1]);
    check_error(groupInfo != NULL, "getgrnam");

    printf("Group name: %s\n", groupInfo->gr_name);
    printf("Group password: %s\n", groupInfo->gr_passwd);
    printf("Group ID: %jd\n", (intmax_t)groupInfo->gr_gid);
    /*
     * Polje gr_mem je niz pokazivača na karaktere (char *), tj. niz stringova.
     *
     * - Svaki element gr_mem[i] pokazuje na C-nisku (string) koja je
     *   terminisana nul-karakterom '\0'.
     *
     * - Ceo niz gr_mem je NULL-terminated, što znači da se kraj niza
     *   označava pokazivačem koji ima vrednost NULL.
     *
     * Razlika između ova dva pojma:
     *
     * 1) Niska (string) terminisana sa '\0':
     *    char s[] = "user";
     *    U memoriji izgleda ovako:
     *      'u' 's' 'e' 'r' '\0'
     *    '\0' označava kraj JEDNE niske.
     *
     * 2) Niz pokazivača terminisan sa NULL:
     *    char *arr[] = {"pera", "mika", "laza", NULL};
     *    U memoriji:
     *      arr[0] -> 'p' 'e' 'r' 'a' '\0'
     *      arr[1] -> 'm' 'i' 'k' 'a' '\0'
     *      arr[2] -> 'l' 'a' 'z' 'a' '\0'
     *      arr[3] -> NULL   <-- kraj niza
     *
     * Zato se kroz gr_mem prolazi petljom dok god pokazivač nije NULL.
     */
    for (int i = 0; groupInfo->gr_mem[i] != NULL; i++) {

        printf("\t%s\n", groupInfo->gr_mem[i]);
    }

    exit(EXIT_SUCCESS);
}
