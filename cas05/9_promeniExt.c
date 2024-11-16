/*
Poziv programa;
./a.out path ext

Program dodaje ekstenziju 'ext' na ime fajla koji se nalazi
na putanji 'path' (slicno kao rename funkcija).
*/

#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <sys/time.h>

#include <string.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

#define MAX_SIZE (256)	

// ./1 path .c
int main(int argc, char** argv) {
	
	check_error(argc == 3, "...");
	
	int strLen = strlen(argv[1]);
	char* eks = strrchr(argv[1], '.');	
	check_error(eks != NULL, "...");
	int eksLen = strlen(eks);
	
	char* newName = malloc(strlen(argv[1]) + strlen(argv[2]) + 1);
	check_error(newName != NULL, "...");
	
	strncpy(newName, argv[1], strLen - eksLen);
	//strcat(newName, ".");
	strcat(newName, argv[2]);
	
	int srcFd = open(argv[1], O_RDONLY);
	check_error(srcFd != -1, "...");
	
	int destFd = open(newName, O_WRONLY|O_TRUNC|O_CREAT);
	check_error(destFd != -1, "...");
	
	struct stat fInfo;
	check_error(fstat(srcFd, &fInfo) != -1, "...");
	
	char buffer[MAX_SIZE];
	int readBytes = 0;
	
	while ((readBytes = read(srcFd, buffer, MAX_SIZE)) > 0) {
		
		check_error(write(destFd, buffer, readBytes) != -1, "....");
	}
	
	check_error(readBytes != -1, "...");
	
	close(srcFd);
	close(destFd);
	
	check_error(unlink(argv[1]) != -1, "...");
	
	check_error(chmod(newName, fInfo.st_mode) != -1, "...");
	
	free(newName);
	
	exit(EXIT_SUCCESS);
}
