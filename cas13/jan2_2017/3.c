#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <sys/wait.h>
#include <ctype.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)
		
#define RD_END 		(0)
#define WR_END 		(1)
#define MAX_SIZE 	(64)	

static char* osUsage = "Usage: ./3";

int main(int argc, char** argv) {
	
	check_error(argc == 1, osUsage);
	
	while (1) {
		
		int parToCld[2];
		int cldToPar[2];
		
		check_error(pipe(parToCld) != -1, "Pipe creation failed");
		check_error(pipe(cldToPar) != -1, "Pipe creation failed");
		
		pid_t childPid = fork();
		check_error(childPid != -1, "Fork failed");
		
		if (childPid) { // parent
			
			char word[MAX_SIZE];
			char command[MAX_SIZE];
			
			char dataToSend[2*MAX_SIZE];
			
			close(parToCld[RD_END]);
			close(cldToPar[WR_END]);
			
			int noOfPars = fscanf(stdin, "%s%s", word, command);
			
			if (noOfPars != 2)
				break;
			
			memset(dataToSend, 0, 2*MAX_SIZE);
			
			if (strlen(command) > 1) {
				fprintf(stderr, "Inappropriate command\n");
				continue;
			}
			
			if (command[0] == 'l' || command[0] == 'u' || command[0] == 'r') {
				sprintf(dataToSend, "%s %s", word, command);
			}
			else {
				fprintf(stderr, "wrong commmand\n");
				continue;
			}
			
			check_error(write(parToCld[WR_END], dataToSend, 2*MAX_SIZE) != -1, "write failed");
			
			memset(dataToSend, 0, 2*MAX_SIZE);
			
			check_error(read(cldToPar[RD_END], dataToSend, 2*MAX_SIZE) != -1, "read failed");
			
			printf("%s\n", dataToSend);
			
			close(parToCld[WR_END]);
			close(cldToPar[RD_END]);
		}
		else { // child
			
			close(parToCld[WR_END]);
			close(cldToPar[RD_END]);
			
			char dataToRec[2*MAX_SIZE];
			memset(dataToRec, 0, 2*MAX_SIZE);
			
			char dataToSend[2*MAX_SIZE];
			memset(dataToSend, 0, 2*MAX_SIZE);
			
			check_error(read(parToCld[RD_END], dataToRec, 2*MAX_SIZE) != -1, "read failed");
			
			int position = (int)(strchr(dataToRec, ' ') - dataToRec);
			
			if (dataToRec[position + 1] == 'l') {
				int i = 0;
				for (i = 0; i < position; i++) {
					dataToSend[i] = tolower(dataToRec[i]);
				}
				
			}
			else if (dataToRec[position + 1] == 'u') {
				int i = 0;
				for (i = 0; i < position; i++) {
					dataToSend[i] = toupper(dataToRec[i]);
				}
			}
			else if (dataToRec[position + 1] == 'r') {
				int i = 0;
				for (i = 0; i < position; i++) {
					dataToSend[i] = tolower(dataToRec[position - i - 1]);
				}
			}
			
			check_error(write(cldToPar[WR_END], dataToSend, 2*MAX_SIZE) != -1, "write failed");
			
			close(parToCld[RD_END]);
			close(cldToPar[WR_END]);
			
			exit(EXIT_SUCCESS);
		}
		
		check_error(wait(NULL) != -1, "wait failed");
	}
	
	exit(EXIT_SUCCESS);
}
