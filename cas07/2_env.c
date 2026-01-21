/*
Program demonstrira rad sa promenljivama okruzenja.
Program se poziva u formatima:
./a.out + name value
./a.out - name
./a.out name

U slucaju da je argv[1] znak +, potrebno je dodati promenljivu sa imenom 'name'
u okruzenje, pri cemu je odgovarajuca vrednost 'value'. U slucaju da je argv[1] znak -
potrebno je izbaciti promenljivu sa imenom 'name' iz okruzenja. Ako nije zadat ni znak +
ni znak -, potrebno je ispisati vrednost promenljive sa imenom 'name'.
*/

#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

/* Eksterna promenljiva koja pokazuje na niz niski oblika "KEY=value"
 * gde je KEY naziv promenljive okruzenja a value njena vrednost
*/
extern char** environ;

/**
 * @brief Funkcija koja stampa sve niske oblika "KEY=value", koje predstavljaju
 * promenljive okruzenja.
 * 
 */
void print_env(void) {
    char **ep;

    for (ep = environ; *ep != NULL; ep++) {
        printf("%s\n", *ep);
    }
}

int main(int argc, char** argv) {

    //print_env();

    check_error(argc >= 2, "./a.out [+-] name [value]");

    if (argv[1][0] == '+') {
        /* postavljamo novu vrednost promenljive okruzenja*/
        check_error(setenv(argv[2], argv[3], 0) != -1, "setenv");
        
        /* citamo vrednost promenljive okruzenja da bismo proverili
            da li je prethodna operacija uspela
        */
        char* value = getenv(argv[2]);
        check_error(value != NULL, "getenv");
        
        printf("%s\n",value);
    }
    else if (argv[1][0] == '-') {
        /* brisemo promenljivu iz okruzenja*/
        check_error(unsetenv(argv[2]) != -1, "unsetenv");

        /* citamo je da bismo proverili uspesnost brisanja */
        char* value = getenv(argv[2]);

        printf("%s\n", value == NULL ? "nema" : "ima");
    }
    else {
        /* citamo vrednost promenljive okruzenja */
        char* value = getenv(argv[1]);

        if(value){
            printf("%s\n", value);
        } else {
            printf("Nema\n");
        }
    }

    exit(EXIT_SUCCESS);
}
