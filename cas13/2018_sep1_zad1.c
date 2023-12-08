#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)
	
int main(int argc, char **argv)
{
	checkError(argc == 3, "...");
	
	time_t sekunde = atoi(argv[1]);
	int min = atoi(argv[2]);
	
	struct tm *t = localtime(&sekunde);
	checkError(t != NULL, "...");
	
	t->tm_min += min;
	time_t novo = mktime(t);
	t = localtime(&novo);
	
	char s[18];
	strftime(s, 18, "%d/%m/%Y %H:%M", t);
	printf("%s\n", s);
	
	/*int dan = t->tm_mday;
	int mesec = t->tm_mon;
	int godina = t->tm_year + 1900;
	int sati = t->tm_hour;
	int minuti = t->tm_min;
	
	if(dan < 10)
		printf("0%d/", dan);
	else
		printf("%d/", dan);
		
	if(mesec < 10)
		printf("0%d/%d ", mesec, godina);
	else
		printf("%d/%d ", mesec, godina);
		
	if(sati < 10)
		printf("0%d:", sati);
	else
		printf("%d:", sati);
		
	if(minuti < 10)
		printf("0%d\n", minuti);
	else
		printf("%d\n", minuti);	*/


	exit(EXIT_SUCCESS);
}