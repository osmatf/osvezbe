/*
Ispisati informacije o fajlovima koji se nalaze u trenutnom direktorijumu.
Program ispisuje informacije o fajlu po uzoru na komandu 'ls -al'.

Poziv programa:
./a.out
*/
#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

#define MAX_SIZE 1024

/*
 * Funkcija vraća korisničko ime (username) na osnovu prosleđenog UID-a.
 *
 * Povratna vrednost:
 *  - Dinamički alociran string koji sadrži ime korisnika.
 *
 * Napomena:
 *  - Pozivalac funkcije je odgovoran za oslobađanje zauzete memorije
 *    pozivom funkcije free().
 */
char *get_username(uid_t user_id)
{
    struct passwd *user_info = getpwuid(user_id);
    check_error(user_info != NULL, "getpwuid");

    char *user_name = malloc(strlen(user_info->pw_name) + 1);
    strcpy(user_name, user_info->pw_name);

    return user_name;
}

/*
 * Funkcija vraća ime grupe (group name) na osnovu prosleđenog GID-a.
 *
 * Povratna vrednost:
 *  - Dinamički alociran string koji sadrži ime grupe.
 *
 * Napomena:
 *  - Pozivalac funkcije je odgovoran za oslobađanje zauzete memorije
 *    pozivom funkcije free().
 */
char *get_groupname(uid_t group_id)
{
    struct group *group_info = getgrgid(group_id);
    check_error(group_info != NULL, "getpwuid");

    char *group_name = malloc(strlen(group_info->gr_name) + 1);
    strcpy(group_name, group_info->gr_name);

    return group_name;
}
/*
 * Funkcija ispisuje informacije o jednom fajlu u formatu sličnom komandi `ls -l`.
 *
 * Sve informacije se najpre formatiraju u dinamički alociran string,
 * koji se zatim ispisuje na standardni izlaz.
 */
void print_info(const char *filename)
{
    /* Alociramo memoriju neophodnu za string */
    char *fileInfoStr = malloc(MAX_SIZE);
    check_error(fileInfoStr != NULL, "alllocation failed");

    /* Citamo metapodatke o fajlu.
     * Koristimo lstat kako bismo dobili informacije o samom simboličkom
     * linku, a ne o fajlu na koji on pokazuje.
     */
    struct stat fInfo;
    check_error(lstat(filename, &fInfo) != -1, "stat failed");

    /* Inicijalno pretpostavljamo da fajl ima sva prava pristupa.
     * Kasnije ćemo ukloniti ona prava koja zapravo ne postoje.
     */
    strcpy(fileInfoStr, "-rwxrwxrwx ");

    /* otkrivamo koji je tip fajla u pitanju
     *
     * man 2 stat i man 7 inode, TLPI za detalje
     */
    if (S_ISREG(fInfo.st_mode)) {
        fileInfoStr[0] = '-';
    } else if (S_ISDIR(fInfo.st_mode)) {
        fileInfoStr[0] = 'd';
    } else if (S_ISLNK(fInfo.st_mode)) {
        fileInfoStr[0] = 'l';
    } else if (S_ISCHR(fInfo.st_mode)) {
        fileInfoStr[0] = 'c';
    } else if (S_ISFIFO(fInfo.st_mode)) {
        fileInfoStr[0] = 'p';
    } else if (S_ISSOCK(fInfo.st_mode)) {
        fileInfoStr[0] = 's';
    } else if (S_ISBLK(fInfo.st_mode)) {
        fileInfoStr[0] = 'b';
    }

    /* otkrivamo koje privilegije poseduje fajl */
    if (!(fInfo.st_mode & S_IRUSR))
        fileInfoStr[1] = '-';
    if (!(fInfo.st_mode & S_IWUSR))
        fileInfoStr[2] = '-';
    if (!(fInfo.st_mode & S_IXUSR))
        fileInfoStr[3] = '-';

    if (!(fInfo.st_mode & S_IRGRP))
        fileInfoStr[4] = '-';
    if (!(fInfo.st_mode & S_IWGRP))
        fileInfoStr[5] = '-';
    if (!(fInfo.st_mode & S_IXGRP))
        fileInfoStr[6] = '-';

    if (!(fInfo.st_mode & S_IROTH))
        fileInfoStr[7] = '-';
    if (!(fInfo.st_mode & S_IWOTH))
        fileInfoStr[8] = '-';
    if (!(fInfo.st_mode & S_IXOTH))
        fileInfoStr[9] = '-';

    /* trenutni broj upisanih karaktera u dinamicki alociranom stringu */
    unsigned offset = 11;

    /* Korišćenjem sprintf upisujemo formatirane informacije u string.
     * Nakon svakog upisa pomeramo offset za broj upisanih karaktera
     * kako ne bismo prepisivali postojeći sadržaj.
     *
     * sprintf vraća broj upisanih bajtova, što koristimo za ažuriranje
     * offset-a.
     */
    offset += sprintf(fileInfoStr + offset, "%ju ", (uintmax_t)fInfo.st_nlink);

    /* Ime korisnika dobijamo na osnovu njegovog UID-a
     * iz passwd baze (/etc/passwd).
     */
    char *userName = get_username(fInfo.st_uid);
    check_error(userName != NULL, "User info error");

    offset += sprintf(fileInfoStr + offset, "%s ", userName);
    free(userName);

    /* Ime grupe dobijamo na osnovu njenog GID-a
     * iz group baze (/etc/group).
     */
    char *groupName = get_groupname(fInfo.st_gid);
    check_error(groupName != NULL, "Group info failed");

    offset += sprintf(fileInfoStr + offset, "%s ", groupName);
    free(groupName);

    /* stampamo velicinu fajla */
    offset += sprintf(fileInfoStr + offset, "%jd ", (intmax_t)fInfo.st_size);

    /* Vreme poslednje izmene fajla.
     * ctime pretvara vreme izraženo u sekundama u čitljiv string.
     */
    time_t seconds = fInfo.st_mtime;
    offset += sprintf(fileInfoStr + offset, "%s ", ctime(&seconds));

    /* Na kraj dodajemo putanju do fajla.
     * Koristimo offset - 2 kako bismo uklonili novi red koji
     * ctime dodaje na kraj stringa.
     */
    offset += sprintf(fileInfoStr + offset - 2, " %s", filename);

    /* Ispisujemo kompletno formatirane informacije */
    printf("%s\n", fileInfoStr);
    return;
}

int main(int argc, char **argv)
{
    check_error(argc == 1, "./a.out");

    DIR *dir = opendir(".");
    check_error(dir != NULL, "opendir");

    struct dirent *dir_entry = NULL;
    errno = 0;
    while ((dir_entry = readdir(dir)) != NULL) {
        print_info(dir_entry->d_name);
        errno = 0;
    }
    check_error(errno == 0, "readdir");

    closedir(dir);

    exit(EXIT_SUCCESS);
}
