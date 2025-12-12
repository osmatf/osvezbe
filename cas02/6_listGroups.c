/*
Program koji ispisuje informacije o svim grupama na sistemu.
*/

#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <grp.h>

#define check_error(cond,userMsg)\
	do {\
		if (!(cond)) {\
			fprintf(stderr,"Greska: %s\n",userMsg);\
			exit(EXIT_FAILURE);\
		}\
	} while (0)

void print_group(struct group* groupInfo) {
	
	printf("***************************************************************\n");
	printf("Group name: %s\n", groupInfo->gr_name);
	printf("Group password: %s\n", groupInfo->gr_passwd);
	printf("Group ID: %d\n", (int)groupInfo->gr_gid);
	for (int i = 0; groupInfo->gr_mem[i] != NULL; i++) {
			
			printf("\t%s\n", groupInfo->gr_mem[i]);
	}
}
int main(int argc, char** argv) {
	
	check_error(argc == 1, "...");
	
	setgrent();
	
	struct group* currGroup = NULL;
	while ((currGroup = getgrent()) != NULL){
		
		print_group(currGroup);
	}
	
	endgrent();
	
	exit(EXIT_SUCCESS);
}
