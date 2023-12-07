/*
Program demonstrira rad sa zakljucavanjem fajlova. Poziv programa:
./lock_file path/to/file start bytes sleepTimeInSec lockType (r or w)

Potrebno je zakljucati deo fajla za citanje/pisanje na putanji 'path/to/file' 
pocev od bajta 'start' duzine 'bytes'. Nakon toga uspavati proces
na 'sleepTimeInSec' sekundi, da bi drugi procesi stigli da probaju 
sa zakljucavanjem.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

static const char *os_Usage = "Usage: ./lock_file path/to/file start bytes sleepTimeInSec lockType (r or w)";

int main(int argc, char **argv) {
    check_error(6 == argc, os_Usage);
    
    /*
     * parsiramo argumente i pretvaramo ih u odgovarajuce tipove
     */
    char *fpath = argv[1];
    int start = atoi(argv[2]);
    int length = atoi(argv[3]);
    int sleepTimeInSec = atoi(argv[4]);
    int lockType = argv[5][0] == 'r' ? F_RDLCK : F_WRLCK;
    
    /*
     * otvaramo fajl u kombinovanom modu, jer korisnik moze da postawi i r i w katanac.
     */
    int fd = open(fpath, O_RDWR);
    check_error(-1 != fd, "Opening file failed");
  
    /* u strukturi flock moramo da navedemo svojstva katanca koji zelimo da postavimo */
    struct flock lock;
    lock.l_type = lockType;
    lock.l_whence = SEEK_SET;
    lock.l_start = start;
    lock.l_len = length;
    printf("Trying to lock...\n");
    /* pokusavamo da zakljucamo fajl na zeljeni nacin
    * F_SETLK - ako ne moze da zakljuca fajl, odmah puca i vraca se nazad
    * F_SETLKW - ako ne moze da zakljuca fajl, sacekace dok se resurs ne oslobodi
    * F_GETLK - se koristi za ispitivanje da li moze da se postavi zeljeni katanac
    *           u ovom slucaju nije lose biti pohlepan i uvek ispitivati maksimalne mogucnosti, tj.
    *           treba postaviti tip katanca na F_WRLCK. Ako fajl moze da se zakljuca u w modu
    *           sigurno moze i u r modu. Vodite se parolom "ko moze vise moze i manje".
    *
    * Kada se vrsi zakljucavanje, uvek treba zakljucavati minimum potrebnih bajtova da bi se omogucio sto
    * veci paralelizam. 
    */
    check_error(-1 != fcntl(fd, F_SETLK, &lock), "Locking file failed");
    printf("Lock acquired...\n");
    
    /*
     * Uspavljujemo proces cime simuliramo drzanje katanca
     */
    sleep(sleepTimeInSec);
    /* svaki postavljeni katanac je neophodno otkljucati u nekom trenutku 
    * tip postavljamo na F_UNLCK
    */
    lock.l_type = F_UNLCK;
    /* I otkljucavamo fajl */
    check_error(-1 != fcntl(fd, F_SETLK, &lock), "Unlocking file failed");
    printf("Lock released...\n");
    /* zatvaramo fajl deskriptor
    * i da zaboravite otkljucavanje, zatvaranjem deskriptora sa automatski oslobadaju svi katanci 
    */
    close(fd);
  
    return 0;
}
