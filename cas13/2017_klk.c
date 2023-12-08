#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <ftw.h>
#include <stdbool.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

bool osIsPublicFile(const char *fpath)
{
    struct stat buffer;
    check_error(stat(fpath, &buffer) != -1, "Nepostojeći fajl");
    check_error((buffer.st_mode & S_IFMT) == S_IFREG, "Pogresan tip fajla");
    if(buffer.st_mode & S_IROTH && buffer.st_mode & S_IWOTH)
        return true;
    return false; 
}

void osMkPublicDir(const char *dname)
{
    mode_t mode = 0777;
    int k = mkdir(dname, mode);
    check_error(k != -1, "Postoji folder");
    check_error(chmod(dname, mode) != -1, "chmod");
}

unsigned osNumOfDaysFileModified(const char *fpath)
{
	time_t now = time(NULL);
	struct stat fInfo;
	check_error(stat(fpath, &fInfo) != -1, "Ne postoji");
	time_t razlika = now - fInfo.st_mtim.tv_sec;
	return razlika / (60 * 60 * 24);
}

void osMoveFile(const char *srcPath, const char *destPath)
{
	check_error(strcmp(srcPath, destPath) != 0, "isti fajl");
	struct stat fInfo;
	check_error(stat(srcPath, &fInfo) != -1, "stat");
	int fdSrc = open(srcPath, O_RDONLY);
	check_error(fdSrc != -1, "open1");
	int fdDest = open(destPath, O_WRONLY | O_CREAT | O_TRUNC);
	check_error(fdDest != -1, "open2");
	int bytesRead = 0;
	char memBuff[1024];
	while ((bytesRead = read(fdSrc, memBuff, 1024)) > 0)
	{
		check_error(write(fdDest, memBuff, bytesRead) != -1, "write");
	}
	check_error(bytesRead != -1, "write");
	close(fdSrc);
	close(fdDest);
	check_error(unlink(srcPath) != -1, "unlink");
	check_error(chmod(destPath, fInfo.st_mode) != -1, "chmod");
}

char newDir[1024];

int processFile(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
	//printf("%s\n", fpath);
	if(S_ISREG(sb->st_mode))
	{
		if(osIsPublicFile(fpath))
		{
			if(osNumOfDaysFileModified(fpath) < 30)
			{
				char *temp = malloc(strlen(newDir) + strlen(fpath + ftwbuf->base) + 2);
				strcpy(temp, newDir);
				strcat(temp, "/");
				strcat(temp, fpath + ftwbuf->base);
				osMoveFile(fpath, temp);
				free(temp);
			}
				
			else
				unlink(fpath);
		}
		// else
		// 	unlink(fpath);
	}
	return 0;
}

int main(int argc, char const *argv[])
{
	check_error(argc == 3, "Argumenti");
	struct stat dir1;
	check_error(stat(argv[1], &dir1) != -1, "ne postoji1");
	check_error(S_ISDIR(dir1.st_mode), "nije dir");

	strcpy(newDir, argv[2]);
	osMkPublicDir(argv[2]);
	
	check_error(nftw(argv[1], processFile, 50, 0) != -1, "nftw");
	//check_error(rmdir(argv[1]) != -1, "rmdir");
	return 0;
}