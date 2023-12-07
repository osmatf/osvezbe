/*
Program demonstrira upotrebu inotify() interfejsa. Prvo kreiramo instancu,
a zatim dodajemo fajlove (direktorijume) koje zelimo da nadgledamo.
Nakon toga, cekamo u beskonacnoj petlji da se neki dogadjaj desi, pri 
cemu nakon nekog desavanja citamo dogadjaje jedan po jedan iz bafera.

Napomena: dogadjaji koji su se dogodili su serijalizovani, odnosno
predstavljeni kao obican niz bajtova, pa ih moramo deserijalizovati,
odnosno izvuci iz tog bafera jedan po jedan.
*/
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


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

/* Ispis informacija iz inotify_event strukture */
static void displayInotifyEvent(struct inotify_event *i)
{
    printf("wd =%2d; ", i->wd);
    if (i->cookie > 0)
        printf("cookie =%4d; ", i->cookie);

    printf("mask = ");
    if (i->mask & IN_ACCESS) printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB) printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE) printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE) printf("IN_CREATE ");
    if (i->mask & IN_DELETE) printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF) printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED) printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR) printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY) printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF) printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM) printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO) printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN) printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW) printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT) printf("IN_UNMOUNT ");
    printf("\n");

    if (i->len > 0)
        printf("         name = %s\n", i->name);
}

/* Dogadjaji su serijalizovani (pretvoreni u niz bajtova) pa ih moramo izvlaciti iz bafera jedan po jedan*/
void process_all_events(char *buf, ssize_t numRead){
    char *p = NULL;

    for (p = buf; p < buf + numRead; ) {
        struct inotify_event *event = (struct inotify_event *) p;
        displayInotifyEvent(event);

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
        int wd = inotify_add_watch(inotifyFd, argv[j], IN_ALL_EVENTS);
        check_error(wd != -1, "add_watch");

        printf("Watching %s using wd %d\n", argv[j], wd);
    }

    char buf[BUF_LEN];
    /* Citamo dogadjaje u beskonacnoj petlji */
    for (;;) {
        ssize_t numRead = read(inotifyFd, buf, BUF_LEN);
        check_error(numRead != -1, "read");
    
        if (numRead == 0)
            printf("read() from inotify fd returned 0!");

        printf("Read %jd bytes from inotify fd\n", (intmax_t) numRead);
        
        /* Process all of the events in buffer returned by read() */
        process_all_events(buf, numRead);
    }

    exit(EXIT_SUCCESS);
}