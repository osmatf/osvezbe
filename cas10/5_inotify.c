/*
Kao argument komandne linije prosledjuje se direktorijum koji treba nadgledati.
Potrebno je izbrojati broj fajlova koji su izmesteni iz tog direktorijuma.
Dogadjaje nadgledati sve dok ne stigne signal SIGINT, kada treba prekinuti
petlju i zavrsiti rad programa ispisivanjem broja izmestenih fajlova.
*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <signal.h>
#include <stdbool.h>
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

int count = 0;
/* Dogadjaji su serijalizovani pa ih moramo izvlaciti iz bafera jedan po jedan*/
void process_all_events(char *buf, ssize_t numRead){
    char *p = NULL;

    for (p = buf; p < buf + numRead; ) {
        struct inotify_event *event = (struct inotify_event *) p;
        if(event->mask & IN_MOVED_FROM){
            count ++;
        }

        p += sizeof(struct inotify_event) + event->len;
    }
}

void obradi_sigint(int signum){
    printf("\nZavrsavam nadgledanje...\n");
    printf("Izmesteno je %d fajlova\n", count);

    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    check_error(argc == 2, "args");

    check_error(signal(SIGINT, obradi_sigint) != SIG_ERR, "signal");

    int inotifyFd = inotify_init();
    check_error(inotifyFd != -1,"inofify_init");

    /* Create inotify instance */
    int wd = inotify_add_watch(inotifyFd, argv[1], IN_MOVED_FROM);
    check_error(wd != -1, "add_watch");

    printf("Watching %s using wd %d\n", argv[1], wd);

    char buf[BUF_LEN];
    /* Citamo dogadjaje u beskonacnoj petlji */
    for (; ;) {
        ssize_t numRead = read(inotifyFd, buf, BUF_LEN);
        check_error(numRead != -1, "read");

        if (numRead == 0)
            printf("read() from inotify fd returned 0!");

        /* Process all of the events in buffer returned by read() */
        process_all_events(buf, numRead);
    }

    exit(EXIT_SUCCESS);
}