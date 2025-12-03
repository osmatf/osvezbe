/*
Program koji ispisuje nazive svih korisnika na sistemu
ciji naziv pocinje sa odredjenim prefiksom. Prefiks se zadaje
kao argument komandne linije.

Poziv programa: ./a.out <prefix>
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 
#include <sys/types.h>
#include <pwd.h>
#include <string.h>

#define check_error(cond, msg)\
do{\
    if(!(cond)){\
        perror(msg);\
        fprintf(stderr,"File:%s\nLine:%d\nFunc:%s\n", __FILE__, __LINE__, __func__);\
        exit(EXIT_FAILURE);\
    }\
}while(0)

int pocinje_sa(const char *name,const char *prefix){
    // ako je prefix duzi od imena, znamo da ime ne pocinje sa prefix-om
    if(strlen(prefix) > strlen(name)){
        return 0;
    }

    // pomeramo pokazivace na prefix i name i poredimo karaktere
    while(*prefix != '\0'){
        if(*prefix != *name){
            return 0;
        }

        prefix++;
        name++;
    }
    // ukoliko su svi karakteri isti, izlazimo iz petlje

    return 1;
}

int main(int argc, char **argv){
    check_error(argc == 2, "argc");
    struct passwd *user_info = NULL;

    setpwent();

    while((user_info = getpwent()) != NULL){
        if(pocinje_sa(user_info->pw_name, argv[1])){
            printf("%s\n", user_info->pw_name);
        }
    }

    endpwent();

    return 0;
}