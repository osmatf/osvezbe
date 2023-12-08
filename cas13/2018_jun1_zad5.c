#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>

#include<semaphore.h>
#include<sys/mman.h>
#include<string.h>
#include<signal.h>
#include<math.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)
    
#define MAX_SIZE (1024)
    
typedef struct {
    sem_t ready;
    sem_t done;
    double points[MAX_SIZE];
    unsigned nPoints; /* broj tacaka */
} OsInputData;


void *getMemBlock(char *fpath, unsigned *size){
    
    int memFd = shm_open(fpath, O_RDWR, 0600);
    check_error(memFd!=-1, "shm_open");
    
    struct stat fInfo;
    check_error(fstat(memFd, &fInfo)!=-1, "fstat");
    
    *size = fInfo.st_size;
    
    void *addr = mmap(NULL, *size, PROT_READ| PROT_WRITE, MAP_SHARED, memFd, 0);
    check_error(addr!=MAP_FAILED, "mmap");
    
    close(memFd);
    
    return addr;
}
    
    
int main(int argc, char **argv){
    
    check_error(argc==2, "argc");
    
    unsigned size = 0;
    
    OsInputData *data = getMemBlock(argv[1], &size);
   
    check_error(sem_wait(&(data->ready))!=-1, "sem_wait");
    
    double *niz = data->points;
    int n = data->nPoints;
    
    int init=0;
    double minRastojanje=0;
    
    for(int i=0; i<n; i++){        
        double i_x=niz[2*i];
        double i_y= niz[2*i+1];
        
        for(int j=i+1; j<n; j++){
            double j_x=niz[2*j];
            double j_y= niz[2*j+1];
            
            double rastojanje = sqrt((j_x-i_x)*(j_x-i_x) + (j_y-i_y)*(j_y-i_y));
            
            if(init==0){
                minRastojanje = rastojanje;
                init=1;
            }
            
            if(rastojanje< minRastojanje)
                minRastojanje=rastojanje;
        }
    }
    
    printf("%lf\n", minRastojanje);
    
    check_error(sem_post(&(data->done))!=-1, "sem_post");
    
    check_error(munmap(data, size)!=-1, "munmap");
    
            
    exit(EXIT_SUCCESS);
}