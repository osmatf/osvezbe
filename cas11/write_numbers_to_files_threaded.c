/*
Program kreira 20 fajlova i upisuje 2 000 000 brojeva u svaki od njih.
Ovaj primer sluzi da demonstrira razliku u brzini izvrsavanja programa
bez i sa koriscenjem niti. Ovaj program koristi niti (za svaki fajl po jednu).
Pokrenuti program i obratiti paznju koliko se brze zavrsava pisanje
u slucaju da svakom fajlu dodelimo po jednu nit.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

#define NUM_OF_FILES 20
#define NUM_OF_NUMBERS 2000000

typedef struct {
    char filename[10];
} ThreadArg;

void *write_numbers_to_file(void *arg){
    char filename[10];
    ThreadArg *argument = (ThreadArg *) arg;
    strcpy(filename, argument->filename);

    FILE *f = fopen(filename, "w");
    check_error(f != NULL, "fopen");

    for(int i = 0; i < 2000000; i++){
        fprintf(f,"%d ", i);
    }

    fclose(f);

    return NULL;
}

int main(){
    pthread_t tids[NUM_OF_FILES];
    ThreadArg args[NUM_OF_FILES];

    printf("Writing numbers...\n");

    for(int i = 0; i < NUM_OF_FILES; i++){
        sprintf(args[i].filename,"%d.txt",i);
       
        pthread_create(&tids[i],NULL,write_numbers_to_file,(void *)&args[i]);
    }

    for(int i = 0; i < NUM_OF_FILES; i++){
        pthread_join(tids[i], NULL);
    }
    printf("Done!\n");

    printf("Deleting files...\n");
    for(int i = 0; i < NUM_OF_FILES; i++){
        check_error(unlink(args[i].filename) != -1, "unlink");
    }
    printf("Done!\n");

    return 0;
}