/*
Kao argumenti komandne linije zadaju se putanje do fajlova.
Ukoliko je zadati fajl direktorijum, treba nadgledati da li je kreiran fajl
u njemu (IN_CREATE). Svaki put kada se kreira novi fajl, potrebno je
ispisati njegova prava pristupa.
Ukoliko je zadati fajl regularan, potrebno je  nadgledati da li
mu je pristupano (IN_ACCESS) ili je modifikovan (IN_MODIFY). Ukoliko
mu je pristupano, potrebno je ispisati vreme pristupa u formatu
HH:MM:SS. Ukoliko je modifikovan, potrebno je ispisati vreme modifikacije
u formatu HH:MM:SS.
*/
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include <sys/inotify.h>
#include <limits.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))


void print_permissions(mode_t mode){
    if(mode & S_IRUSR){ printf("r");} else {printf("-");}
    if(mode & S_IWUSR){ printf("w");} else {printf("-");}
    if(mode & S_IXUSR){ printf("x");} else {printf("-");}
    if(mode & S_IRGRP){ printf("r");} else {printf("-");}
    if(mode & S_IWGRP){ printf("w");} else {printf("-");}
    if(mode & S_IXGRP){ printf("x");} else {printf("-");}
    if(mode & S_IROTH){ printf("r");} else {printf("-");}
    if(mode & S_IWOTH){ printf("w");} else {printf("-");}
    if(mode & S_IXOTH){ printf("x");} else {printf("-");}
    putchar('\n');
}

static void process_event(struct inotify_event *i, char **argv)
{
    struct stat fileinfo;

    if (i->mask & IN_ACCESS){
        check_error(stat(argv[i->wd], &fileinfo) != -1, "stat");

        struct tm *vremena = localtime(&fileinfo.st_atime);
        check_error(vremena != NULL, "localtime");

        printf("[ACCESS] %d:%d:%d\n", vremena->tm_hour, vremena->tm_min, vremena->tm_sec);
    } else if (i->mask & IN_MODIFY) {
        check_error(stat(argv[i->wd], &fileinfo) != -1, "stat");

        struct tm *vremena = localtime(&fileinfo.st_atime);
        check_error(vremena != NULL, "localtime");

        printf("[MODIFY] %d:%d:%d\n", vremena->tm_hour, vremena->tm_min, vremena->tm_sec);
    }
    else if (i->mask & IN_CREATE) {
        char *path = malloc(strlen(argv[i->wd]) + strlen(i->name) + 2);
        strcpy(path, argv[i->wd]);
        strcat(path, "/");
        strcat(path, i->name);

        check_error(stat(path, &fileinfo) != -1, "stat");
        printf("[CREATE] %s: ", i->name);

        print_permissions(fileinfo.st_mode);
    }

}

/* Dogadjaji su serijalizovani (pretvoreni u niz bajtova) pa ih moramo izvlaciti iz bafera jedan po jedan*/
void process_all_events(char *buf, ssize_t numRead, char **argv){
    char *p = NULL;

    for (p = buf; p < buf + numRead; ) {
        struct inotify_event *event = (struct inotify_event *) p;
        
        process_event(event, argv);

        p += sizeof(struct inotify_event) + event->len;
    }
}

int main(int argc, char *argv[])
{
    check_error(argc >= 2, "args");

    /* Kreiramo inotify instancu */
    int inotifyFd = inotify_init();
    check_error(inotifyFd != -1,"inofify_init");

    /* dodajemo dogadjaje za nadgledanje u instancu */
    for (int j = 1; j < argc; j++) {
        struct stat fileinfo;
        check_error(stat(argv[j], &fileinfo) != -1, "stat");
        int wd;
        if(S_ISDIR(fileinfo.st_mode)){
            wd = inotify_add_watch(inotifyFd, argv[j], IN_CREATE);
            check_error(wd != -1, "add_watch");
        } else if(S_ISREG(fileinfo.st_mode)){
            wd = inotify_add_watch(inotifyFd, argv[j], IN_ACCESS | IN_MODIFY);
            check_error(wd != -1, "add_watch");
        } else {
            check_error(0, "Wrong file type");
        }

        printf("Watching %s using wd %d\n", argv[j], wd);
    }

    char buf[BUF_LEN];
    /* Citamo dogadjaje u beskonacnoj petlji */
    for (;;) {
        ssize_t numRead = read(inotifyFd, buf, BUF_LEN);
        check_error(numRead != -1, "read");
    
        if (numRead == 0)
            printf("read() from inotify fd returned 0!");
        
        /* Process all of the events in buffer returned by read() */
        process_all_events(buf, numRead, argv);
    }

    exit(EXIT_SUCCESS);
}