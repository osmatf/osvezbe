#define _XOPEN_SOURCE 500
#include <errno.h>
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

int main(int argc, char **argv)
{
    check_error(argc == 1, "./a.out");

    setpwent();

    struct passwd *user_info = NULL;
    /* citamo korisnike */
    while ((user_info = getpwent()) != NULL) {
        char *home_dir_path = user_info->pw_dir;
        /* ako neki korisnik ima kao home direktorijum '/', preskacemo ga*/
        if (strcmp(home_dir_path, "/") != 0) {
            /* izdvajamo naziv home direktorijuma */
            char *homedir_name = strrchr(home_dir_path, '/');

            if (homedir_name != NULL) {
                homedir_name += 1;

                /* postavljamo umask na 0 da bismo omogucili sve privilegije */
                int old_umask = umask(0);

                errno = 0;
                mkdir(homedir_name, 0777);
                /* u slucaju da direktorijum vec postoji, samo nastavljamo dalje
                 * odnosno ne zelimo da program pukne
                 */
                if (errno != 0 && errno != EEXIST) {
                    check_error(0, "mkdir");
                }

                /* vracamo stari umask*/
                umask(old_umask);
            }
        }
    }

    endpwent();

    return 0;
}