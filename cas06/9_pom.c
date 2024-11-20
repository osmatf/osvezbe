#include <stdio.h>
#include <stdlib.h>

#define check_error(cond, msg)\
        do{\
          if(!(cond)){\
            perror(msg);\
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
            exit(EXIT_FAILURE);\
          }\
        }while(0)

int main(int argc, char **argv){
  check_error(argc == 2, "./a.out 123");

  printf("%s\n", argv[1]);

  return 0;
}








