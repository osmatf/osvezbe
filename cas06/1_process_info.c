/*
Program ispisuje PID trenutnog procesa i PID roditeljskog procesa.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

int main(int argc, char** argv) {
	
	/* Podsetimo se, proces je program u izvrsavanju.
     * Na primer, kada neki a.out program pokrenemo sa ./a.out
     * on postaje proces i dobija svoj identifikator na sistemu,
     * odnosno PID.
     * 
     * Komandom `ps aux` u terminalu mozete dobiti spisak svih trenutnih procesa na sistemu.
     */
	printf("Current process ID: PID=%jd\n", (intmax_t)getpid());
    
    /* Osim toga, svaki proces mora biti pokrenut od strane nekog drugog procesa 
     * (osim prvog, odnosno INIT procesa). Mi to uglavnom radimo pomocu shell-a,
     * kog pokrece sistem kada kliknemo na terminal (ili ctrl + alt + t).
     * Stoga za svaki proces je dostupan i PID njegovog roditelja.
     */
    printf("Current process parent's ID: PPID=%jd\n", (intmax_t)getppid());
	
	exit(EXIT_SUCCESS);
}
