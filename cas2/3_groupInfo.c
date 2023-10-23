/*
Program treba da ispise informacije o grupi 'groupname',
pri cemu se informacije dobijaju iz group fajla.

Poziv programa:
./groupInfo groupname
*/

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
	
// ./groupInfo grpName
int main(int argc, char** argv) {
	
	check_error(argc == 2, "args");
	
	struct group* groupInfo = getgrnam(argv[1]);
	check_error(groupInfo != NULL, "...");
	
	printf("Group name: %s\n", groupInfo->gr_name);
	printf("Group password: %s\n", groupInfo->gr_passwd);
	printf("Group ID: %d\n", (int)groupInfo->gr_gid);
	for (int i = 0; groupInfo->gr_mem[i] != NULL; i++) {
			
			printf("\t%s\n", groupInfo->gr_mem[i]);
	}

	exit(EXIT_SUCCESS);
}
