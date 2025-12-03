/*
Ispisati broj clanova u grupi ciji se naziv zadaje kao argument komandne linije.

Poziv programa: ./a.out <group_name>
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 
#include <sys/types.h>
#include <pwd.h>
#include <stdint.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define check_error(cond, msg)\
do{\
    if(!(cond)){\
        perror(msg);\
        fprintf(stderr,"File:%s\nLine:%d\nFunc:%s\n", __FILE__, __LINE__, __func__);\
        exit(EXIT_FAILURE);\
    }\
}while(0)

int broj_clanova(const char *group_name){
    int brojac = 0;

    const struct group *group_info = getgrnam(group_name);
    check_error(group_info != NULL, "getgrnam");

    while(group_info->gr_mem[brojac] != NULL){
        brojac++;
    }

    return brojac;
}

int main(int argc, char **argv){
    check_error(argc == 2, "./a.out <group_name>");

    int br = broj_clanova(argv[1]);
    printf("Broj clanova u grupi '%s' je %d\n",argv[1], br);

    return 0;
}