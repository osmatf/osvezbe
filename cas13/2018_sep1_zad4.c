#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include<poll.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

#define MAX_SIZE (1024)

int main(int argc, char *argv[])
{
	check_error(argc > 1, "argumenti");

	int nFds = argc - 1;

	struct pollfd *fds =  malloc(nFds * sizeof(struct pollfd));
	check_error(fds != NULL, "malloc");

	for(int i = 0; i < nFds; i++){

		int fd = open(argv[i+1], O_RDONLY | O_NONBLOCK);
		check_error(fd != -1, "open");

		fds[i].fd = fd;
		fds[i].events = POLLIN | POLLERR | POLLHUP;
		fds[i].revents = 0;
	}

	int activeFds = nFds;
	char buffer[MAX_SIZE];
	

	int maxA = -1;
	int idx = -1;

	while(activeFds){

		check_error(poll(fds,nFds,-1) != -1, "poll");

		for(int i = 0; i < nFds; i++){

			if(fds[i].revents & POLLIN){

				int readBytes = read(fds[i].fd, buffer, MAX_SIZE);
				check_error(readBytes != -1, "read");

				int br = 0;
				for(int j = 0; j < readBytes; j++){
					if(buffer[j] == 'a')
						br++;
				}

				if(br > maxA){
					maxA = br;
					idx = i;
				}
			}

			else if(fds[i].revents & (POLLERR | POLLHUP)){

				close(fds[i].fd);

				fds[i].fd = -1;
				fds[i].events = 0;
				fds[i].revents = 0;

				activeFds--;
			}
		}

	}


	char* ime = strrchr(argv[idx+1], '/');
	printf("%s %d\n", ime ==  NULL ? argv[idx+1] : ime + 1, maxA);

	free(fds);

	exit(EXIT_SUCCESS);
}