#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

void obidji_dir(char* path, int print)
{
	struct stat fInfo;
	check_error(lstat(path, &fInfo) != -1, "..");
	
	if(S_ISREG(fInfo.st_mode)) {
	
		if(print) {
		
			char* name = strrchr(path, '/');
			if(name == NULL)	
				name = path;
			else
				name = name+1;	
		
			printf("%s\n", name);	
			
		}
		return;
	}
	
	if(!S_ISDIR(fInfo.st_mode))
		return;
		
	DIR* dir = opendir(path);
	check_error(dir != NULL, "..");
	
	struct dirent* entry = NULL;

	while((entry = readdir(dir)) != NULL) {
	
		if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			continue;
			
		char* new = malloc(strlen(path) + strlen(entry->d_name) + 2);
		check_error(new != NULL, "..");
		
		sprintf(new, "%s/%s", path, entry->d_name);
		
		char* imedir = strrchr(path, '/');
		if(imedir == NULL)
			imedir = path;
		else
			imedir = imedir+1;
		
		if (print)
			obidji_dir(new, 1);
		else if(!strncmp("dir_", imedir, 4))
			obidji_dir(new, 1);
		else
			obidji_dir(new, 0);
		
		free(new);
	}
	

	closedir(dir);

}

int main(int argc, char** argv){
	
	check_error(argc == 2, "..");
	
	struct stat fInfo;
	check_error(lstat(argv[1], &fInfo) != -1, "..");
	check_error(S_ISDIR(fInfo.st_mode), "..");
	
	obidji_dir(argv[1], 0);

	
	exit(EXIT_SUCCESS);
}