/*
Napisati:
a) Funckiju koja na osnovu user ID-a vraca njegovo ime.

b) Funkciju koja na osnovu group ID-a vraca njeno ime.

c) Program koji koriscenjem prethodne dve funckije
ispisuje naziv vlasnika i grupe vlasnika za fajl cija se
putanja dobija kao argument komandne linije.

Poziv programa:
./fileinfo filePath
*/
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

/**
 * Funkcija pronalazi korisničko ime koje odgovara zadatom
 * korisničkom identifikatoru (uid) korišćenjem funkcije getpwuid().
 *
 * Funckija vraća pokazivač na dinamički alociranu nisku koja sadrži
 *         korisničko ime. Pozivalac je odgovoran za oslobađanje
 *         memorije pozivom free().
 *
 * Funkcija getpwuid() vraća pokazivač na strukturu čiji se
 * sadržaj nalazi u statičkoj memoriji biblioteke. Polje
 * pw_name može biti prepisano narednim pozivima getpwuid(),
 * getpwnam() i sličnih funkcija.
 *
 * Zbog toga se korisničko ime kopira u novu, dinamički
 * alociranu nisku, kako bi se obezbedilo da vraćeni podaci
 * ostanu validni nezavisno od drugih poziva i da postoji
 * jasno vlasništvo nad memorijom.
 */
char *get_username(uid_t user_id)
{
    struct passwd *user_info = getpwuid(user_id);
    check_error(user_info != NULL, "getpwuid");

    char *user_name = malloc(strlen(user_info->pw_name) + 1);
    strcpy(user_name, user_info->pw_name);

    return user_name;
}

/* analogno funkciji get_username */
char *get_groupname(uid_t group_id)
{
    struct group *group_info = getgrgid(group_id);
    check_error(group_info != NULL, "getpwuid");

    char *group_name = malloc(strlen(group_info->gr_name) + 1);
    strcpy(group_name, group_info->gr_name);

    return group_name;
}

int main(int argc, char **argv)
{
    check_error(argc == 2, "./a.out <filepath>");

    struct stat fileinfo;
    check_error(stat(argv[1], &fileinfo) != -1, "stat");

    /*
     * Struktura stat ne sadrži imena korisnika i grupa, već samo
     * njihove numeričke identifikatore (UID i GID).
     *
     * Da bi se dobio čitljiv naziv korisnika i grupe, neophodno je
     * izvršiti mapiranje UID/GID → ime, što se postiže pozivima
     * getpwuid() i getgrgid().
     */
    char *username = get_username(fileinfo.st_uid);
    char *groupname = get_groupname(fileinfo.st_gid);

    printf("%s %s\n", username, groupname);

    free(username);
    free(groupname);

    exit(EXIT_SUCCESS);
}
