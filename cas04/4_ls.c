/* Program ispisuje nazive svih fajlova unutar trenutnog direktorijuma.
 * Podsetnik: relativna putanja do trenutnog direktorijuma je "."
 *
 * Primer poziva:
 * ./my_ls
 */
#define _XOPEN_SOURCE 700
#include <sys/types.h>
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
    check_error(argc == 1, "./a.out");

    /* otvaramo trenutni direktorijum */
    DIR *dir_stream = opendir(".");
    check_error(dir_stream != NULL, "opendir");

    struct dirent *dir_entry = NULL;
    errno = 0;
    while((dir_entry = readdir(dir_stream)) != NULL){
        /* "." i ".." su podrazumevani fajlovi unutar svakog direktorijuma
         * pa njih preskacemo.
         */
        if(strcmp(dir_entry->d_name, ".") != 0 && strcmp(dir_entry->d_name, "..") != 0){
            printf("%s\n", dir_entry->d_name);
        }
    }
    /* kada izadjemo iz petlje ne znamo da li je to zato sto smo obisli ceo direktorijum
     * ili zato sto je nastala greska pri citanju. Zato moramo obraditi errno 
     */
    check_error(errno == 0, "readdir");
    /* zatvaramo direktorijum*/
    closedir(dir_stream);

	exit(EXIT_SUCCESS);
}
