#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#include <sys/wait.h>
#include <pthread.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

#define osCheckPthread(pthreadErr, userMsg) \
    do { \
      int __pthreadErr = (pthreadErr); \
      if (__pthreadErr) { \
        errno = __pthreadErr; \
        check_error(false, userMsg); \
      } \
    } while (0)
    
static const char *os_Usage = "./3 pathToFile";

typedef struct {
	int row;
} osThreadFuncArgumentType;

double globalSum = 0;
pthread_mutex_t globalLock;

int N = 0;
double** matrixA;
double** matrixB;

void* osThreadFunc(void* arg) {
	
	osThreadFuncArgumentType* args = (osThreadFuncArgumentType*) arg;
	
	double localSum = 0;
	int i;
	
	for (i = 0; i < N; i++) {
		
		localSum += matrixA[args->row][i]*matrixB[args->row][i];
	}
	
	osCheckPthread(pthread_mutex_lock(&globalLock), "mutex lock failed");
	globalSum+=localSum;
	osCheckPthread(pthread_mutex_unlock(&globalLock),"mutex unlock failed");
	
	return NULL;
}

int main(int argc, char** argv) {
	
	check_error(argc == 2, os_Usage);
	
	FILE* f = fopen(argv[1],"r");
	check_error(f != NULL, "file open failed");
	
	fscanf(f, "%d", &N);
	
	int i = 0, j = 0;
	matrixA = malloc(N * sizeof(double*));
	check_error(matrixA != NULL, "allocation failed");
	matrixB = malloc(N * sizeof(double*));
	check_error(matrixB != NULL, "allocation failed");
	for (i = 0; i < N; i++) {
		matrixA[i] = malloc(N*sizeof(double));
		check_error(matrixA[i] != NULL, "allocation failed");
		matrixB[i] = malloc(N*sizeof(double));
		check_error(matrixB[i] != NULL, "allocation failed");
	}
	
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++){
			fscanf(f, "%lf", &matrixA[i][j]);
		}
	}
	
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++){
			fscanf(f, "%lf", &matrixB[i][j]);
		}
	}
	
	fclose(f);
	
	pthread_t* tids = malloc(N*sizeof(pthread_t));
	check_error(tids != NULL, "tids allocation failed");
	osThreadFuncArgumentType *args = malloc(N*sizeof(osThreadFuncArgumentType));
	check_error(args != NULL, "args allocation failed");
	
	osCheckPthread(pthread_mutex_init(&globalLock, NULL), "mutex init failed");
	
	for (i = 0; i < N; i++) {
		args[i].row = i;
		osCheckPthread(pthread_create(&tids[i], NULL, osThreadFunc, (void*)&args[i]), "Thread creation failed");
	}
	
	for (i = 0; i < N; i++) {
		
		osCheckPthread(pthread_join(tids[i], NULL), "Thread joining failed");
	}
	
	osCheckPthread(pthread_mutex_destroy(&globalLock), "mutex lock destroy failed");
	
	printf("%lf\n", globalSum);
	
	for (i = 0; i < N; i++) {
		free(matrixA[i]);
		free(matrixB[i]);
	}
		
	free(matrixA);
	free(matrixB);
	free(tids);
	free(args);
	
	exit(EXIT_SUCCESS);
}
