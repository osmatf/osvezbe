/*
Apsolutne i relativne putanje.

Program za putanju do fajla zadatu kao argument komandne linije ispisuje
apsolutnu (ujedno i kanonsku) putanju do tog fajla.

Poziv programa:
./a.out putanja

APSOLUTNA putanja pocinje znakom '/' i razresava se od korena fajl sistema,
bez obzira na to odakle je program pokrenut.

RELATIVNA putanja ne pocinje znakom '/' i razresava se u odnosu na TEKUCI
DIREKTORIJUM PROCESA (current working directory). To je direktorijum iz kog
je program pokrenut, a NE direktorijum u kom se nalazi izvrsni fajl:

    cd /tmp && ~/os/a.out 1.txt     # trazi se /tmp/1.txt, jer smo program pokrenuli iz /tmp direktorijuma, a ne ~/os/1.txt

Svaki direktorijum u sebi sadrzi dve posebne stavke (pored ostalih fajlova):
    .   - sam taj direktorijum
    ..  - njegov roditeljski direktorijum (u korenu, /.. je opet /)
Zato ih kasnije, pri citanju sadrzaja direktorijuma (readdir), preskacemo.

Kernel putanju razresava deo po deo, s leva na desno. Vise uzastopnih kosih
crta se tretira kao jedna, a kosa crta na kraju je dozvoljena samo ako je
u pitanju direktorijum.

Primeri (neka je npr. tekuci direktorijum /home/student/os):

  putanja                        vrsta       razresava se u
  -----------------------------  ----------  --------------------------------
  /etc/passwd                    apsolutna   /etc/passwd
  cas02/1.txt                    relativna   /home/student/os/cas02/1.txt
  ./cas02/1.txt                  relativna   /home/student/os/cas02/1.txt
  .                              relativna   /home/student/os
  ..                             relativna   /home/student
  ../..                          relativna   /home
  ../../../..                    relativna   /    (iz korena se ne ide dalje)
  cas02/../cas03/./3_mkdir.c     relativna   /home/student/os/cas03/3_mkdir.c
  /usr/./bin/../lib              apsolutna   /usr/lib
  /home//student///os/           apsolutna   /home/student/os

Na primer ako probamo da nadovežemo nešto na fajl koji 
nije direktorijum "fajl.txt/..", kernel vracagresku ENOTDIR, 
jer fajl.txt nije direktorijum.
*/
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
    check_error(argc == 2, "./a.out putanja");

    /* realpath razresava putanju preko fajl sistema: prati simbolicke
     * linkove i zahteva da svi delovi putanje postoje. Sa NULL kao drugim
     * argumentom sam alocira memoriju za rezultat, pa je oslobadjamo na kraju programa.
     */
    char *kanonska = realpath(argv[1], NULL);
    check_error(kanonska != NULL, "realpath");

    printf("realpath: %s\n", kanonska);
    free(kanonska);

    exit(EXIT_SUCCESS);
}
