/* Program ispisuje nazive svih fajlova unutar direktorijuma cija se 
 * putanja proslednjuje kao argument komandne linije. Uz naziv fajla,
 * ispisati i ime vlasnika tog fajla u formatu:
 * <naziv_fajla> <ime_vlasnika>
 *
 * Primer poziva:
 * ./my_ls
 */
#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <dirent.h>
#include <string.h>

#define check_error(cond,userMsg)\
	do {\
		if (!(cond)) {\
			perror(userMsg);\
			exit(EXIT_FAILURE); \
		}\
	} while(0)

int main(int argc, char** argv) {
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
    while((dir_entry = readdir(dir_stream)) != NULL){
        if(strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0){
            continue;
        }
        
        /* Da bismo saznali ime vlasnika, potrebno je da uradimo stat, a potom getpwuid.
         * S obzirom da nas program ima trenutni radni direktorijum koji se potencijalno
         * razlikuje od direktorijuma koji se izlistava, neophodno je da formiramo 
         * putanju do fajla koji zelimo da stat-ujemo.
         */
        /* <duzina putanje do direktorijuma> + <1 za '/'> + <duzina naziva fajla> + <1 za term. nulu> */
        char *file_path = malloc(strlen(argv[1]) + 1 + strlen(dir_entry->d_name) + 1);
        check_error(file_path != NULL, "malloc");

        strcpy(file_path, argv[1]);
        strcat(file_path, "/");
        strcat(file_path, dir_entry->d_name);

        /* stat-ujemo fajl da bismo dobili njegov uid */
        struct stat fileinfo;
        check_error(lstat(file_path, &fileinfo) != -1, "lstat");
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
