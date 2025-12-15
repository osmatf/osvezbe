/* Program ispisuje nazive svih fajlova unutar direktorijuma cija se
 * putanja proslednjuje kao argument komandne linije. Uz naziv fajla,
 * ispisati i ime vlasnika tog fajla u formatu:
 * <naziv_fajla> <ime_vlasnika>
 *
 * Primer poziva:
 * ./my_ls
 */
#define _XOPEN_SOURCE 700
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <errno.h>
#include <string.h>

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
    check_error(argc == 2, "./a.out dirpath");

    /* Kada pokrenemo neki program on ima svoj trenutni radni direktorijum (cwd).
     * To je direktorijum od koga polaze sve relativne putanje koje se koriste
     * u programu. Dakle, ako je program pokrenut iz direktorijuma /home/user/Desktop
     * i taj isti program koristi relativnu putanju dir/1.txt onda program gleda fajl
     * na putanji /home/user/Desktop/dir/1.txt.
     *
     * Trenutni radni direktorijum u nasem slucaju program nasledjuje od terminala iz
     * kog pokrecemo program. U terminalu, trenutni radni direktorijum mozete dobiti pomocu
     * komande pwd.
     *
     */
    DIR *dir_stream = opendir(argv[1]);
    check_error(dir_stream != NULL, "opendir");

    struct dirent *dir_entry = NULL;
    errno = 0;
    while ((dir_entry = readdir(dir_stream)) != NULL) {
        if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0) {
            continue;
        }

        /* readdir() nam vraća samo ime stavke u direktorijumu (d_name),
         * ali NE i punu putanju do nje.
         *
         * Poziv stat() uvek radi u odnosu na trenutni radni direktorijum
         * procesa. Ako direktorijum koji listamo (argv[1]) NIJE trenutni
         * radni direktorijum, prosleđivanje samo imena fajla bi dovelo do greške
         * (fajl se ne bi pronašao).
         *
         * Zato je neophodno da formiramo punu putanju u obliku:
         *   <putanja_do_direktorijuma>/<ime_fajla>
         */
        char *file_path = malloc(strlen(argv[1]) + 1 + strlen(dir_entry->d_name) + 1);
        check_error(file_path != NULL, "malloc");

        strcpy(file_path, argv[1]);
        strcat(file_path, "/");
        strcat(file_path, dir_entry->d_name);

        /* stat-ujemo fajl da bismo dobili njegov uid */
        struct stat fileinfo;
        check_error(stat(file_path, &fileinfo) != -1, "stat");
        free(file_path);

        struct passwd *user_info = getpwuid(fileinfo.st_uid);
        check_error(user_info != NULL, "getpwuid");

        printf("%s %s\n", dir_entry->d_name, user_info->pw_name);

        errno = 0;
    }
    check_error(errno == 0, "readdir");

    closedir(dir_stream);

    exit(EXIT_SUCCESS);
}
