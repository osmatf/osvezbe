/*
Program demonstrira kako od FILE * mozemo dobiti odgovarajuci
file descriptor i nadalje raditi operacije nad tim fd.
*/

#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/wait.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

static const char *os_Usage = "Usage: ./stdio_syscalls_connection path/to/reg/file";

#define PIPE_RD (0)
#define PIPE_WR (1)
#define MAX_MSG_LEN (256)

int main(int argc, char **argv) {
    check_error(2 == argc, os_Usage);
    
    printf("C STDIO to SYSCALLS demonstration:\n===================================\n");
    
    /*
     * otvaramo fajl pomocu fopen 
     */
    FILE *fileDem = fopen(argv[1], "r");
    check_error(NULL != fileDem, "Opening file failed");
	/* iz postojeceg streama dohvatamo fajl deskriptor */
    int fileDemFd = fileno(fileDem);
    check_error(-1 != fileDemFd, "Getting file descriptor from FILE* failed");
	/* upotrebom fstata nad izvucenim fajl deskriptorom citamo informacije o fajlu */
    struct stat finfo;
    check_error(-1 != fstat(fileDemFd, &finfo), "Getting info through file descriptor failed");
    
    /*
     * Stampamo velicinu fajla
     */
    printf("File size ('%s'): %jd\n", argv[1], (intmax_t)finfo.st_size);
	/* Zatvaranjem fajl strima automatski se zatvara i fajl deskriptor kome je strim pridruzen, tj.
	 * dovoljno je pozvati fclose().
	 *
	 * BITNO:
	 * zatvaranje fajl deskriptora (close(fd)) nad otvorenim strimom dovodi do nedefinisanog ponasanja programa
	 * i to ne treba nikada raditi
	 */
    fclose(fileDem);

    return 0;
}
