/* Pod baferisanjem se podrazumeva definisanje onoga sto vrsi okidanje fizicke IO operacije. 
	 * Nebaferisani IO znaci da je okidac za IO operacije sama operacija citanja ili pisanja. (default za stderr) 
	 * Linijski baferisani IO znaci da je okidac za IO operacije karakter za novi red. (default za terminale)
	 * Potpuno baferisani IO znaci da je okidac za IO operacije napunjenost bafera. Fizicka IO operacija se vrsi samo onda kada
	 * se stigne do kraja bafera. (default za regularne fajlove)
	 *
	 *
     * Mod baferisanja moze da se kontrolise pomocu funkcije setvbuf:
     * 1) setvbuf(stdout, NULL, _IONBF, 0); -> magic message ide na stdout [nebaferisani IO]
     * 2) setvbuf(stdout, NULL, _IOLBF, 0); -> magic message ide u fajl [linijski baferisani IO]
     * 3) setvbuf(stdout, NULL, _IOFBF, 0); -> magic message ide u fajl [potpuno baferisani IO]
     * Ako se doda novi red na kraj magic message, onda ce ova poruka ici u fajl samo u slucaju 3.
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

static const char *os_Usage = "Usage: ./stdio_buffering path/to/reg/file";

int main(int argc, char **argv) {
    check_error(2 == argc, os_Usage);
    
    /*
     * U slucaju linijskog baferisannja ova poruka se ne stampa na stdout, zbog izostanka novog reda.
     * Pamti se u lokalnom baferu printf-a sve dok se ne pojavi novi red ili se odradi eksplicitno fflush()
     */
    const char *magicMessage = "Magic happens to this message!";
    //printf("%s", magicMessage);
    
    /*
     * Otvaramo fajl na koji cemo preusmeriti stdout
     */
    int fd = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, 0644);
    check_error(-1 != fd, "Opening file failed");
    
    /*
     * Pomocu funkcije dup pravimo kopiju fajl deskriptora za stdout.
	 * Ovo radimo da bismo kasnije mogli da se vratimo na pocetno stanje.
     */
    int fdCpy = dup(STDOUT_FILENO);
    check_error(-1 != fdCpy, "Copying stdout fd failed");
    
    /*
     * Preusmeravamo fajl deskriptore. Sada ce fajl izigravati stdout.
	 * BITNO:
	 * obratiti paznju na upotrebu dup2
     */
    check_error(dup2(fd, STDOUT_FILENO), "Redirecting stdout to file failed");
    printf("This message will go to file along with magic message!\n");
	
	/* 
	 * Funkcija fflush se koristi za rucno praznjenje bafera. Argument NULL znaci
	 * da se prazne svi baferi
	 */
    //fflush(NULL); 

    /*
     * Vracamo se na pocetno stanje, tj. terminal ponovo postaje stdout
	 * BITNO:
	 * obratiti paznju na upotrebu dup2
     */
    check_error(-1 != dup2(fdCpy, STDOUT_FILENO), "Restoring stdout failed");
    
    /*
     * Stampamo poruku na stdout. (terminal)
     */
    printf("Finally stdout!\n");
	
	/* zatvaramo fajl deskriptor */
	close(fd);
    
    return 0;
}
