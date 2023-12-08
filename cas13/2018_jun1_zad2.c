#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <errno.h>

#include <ctype.h>

#define ALPHABETH_LENGTH 26
#define FILENAME_LENGTH 256

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)
	
#define check_pthread(errNum, usrMsg)\
	do {\
		int _err = errNum;\
		if(_err > 0) {\
			errno = _err;\
			check_error(0, usrMsg);\
		}\
	} while(0)
	
int histogram[ALPHABETH_LENGTH];
char filePath[FILENAME_LENGTH];

int maxBrPojavljivanja;
char maxSlovo;
pthread_mutex_t globalLock = PTHREAD_MUTEX_INITIALIZER;
	
typedef struct {
	int idx;
} inputData_t;
	
typedef struct {
	int broj;
} outputArg;

void* threadFunc(void* arg) {
	inputData_t* data = (inputData_t*)arg;
	FILE* f_in = fopen(filePath, "r");
	check_error(f_in != NULL, "...");
	
	char c = 0;
	int localNum = 0;
	while((c = fgetc(f_in)) != EOF){
		if(tolower(c) == data->idx + 'a') {
			localNum++;
		}
	}
	
	check_pthread(pthread_mutex_lock(&globalLock), "...");
	if(localNum > maxBrPojavljivanja) {
		maxBrPojavljivanja = localNum;
		maxSlovo = 'a'+data->idx;
	}
	check_pthread(pthread_mutex_unlock(&globalLock), "...");
	
	histogram[data->idx] = localNum;
	
	fclose(f_in);
	
	outputArg* broj = malloc(sizeof(outputArg));
	broj->broj = localNum;

	pthread_exit(broj);
}
	
int main(int argc, char** argv) {
	check_error(argc == 2, "...");
	
	pthread_t* tids = malloc(ALPHABETH_LENGTH*sizeof(pthread_t));
	check_error(tids != NULL, "...");
	
	inputData_t* args = malloc(ALPHABETH_LENGTH*sizeof(inputData_t));
	check_error(args != NULL, "...");
	
	void **outputs = malloc(ALPHABETH_LENGTH * sizeof(void *));

	strcpy(filePath,argv[1]);
	maxBrPojavljivanja = -1;
	maxSlovo = '\0';
	
	for(int i=0; i<ALPHABETH_LENGTH; i++) {
		args[i].idx = i;
		check_pthread(pthread_create(&tids[i], NULL, threadFunc, &args[i]), "...");
	}


	for(int i=0; i<ALPHABETH_LENGTH; i++) {
		check_pthread(pthread_join(tids[i], &outputs[i]), "...");
	}
	
	for(int i=0; i<ALPHABETH_LENGTH; i++) {
		printf("%d ", ((outputArg *) outputs[i])->broj);
	}
	printf("\n%c\n", maxSlovo);
	
	pthread_mutex_destroy(&globalLock);
	free(tids);
	free(args);
	
	exit(EXIT_SUCCESS);
}