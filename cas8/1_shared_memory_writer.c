/*
Poziv programa:
./shared_memory_writer pathToFile num0 num1 num2 ...

Program prvo kreira objekat deljene memorije na putanji 'pathToFIle'
a potom u nju upisuje niz brojeva num0 num1 num2...
*/
#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <sys/mman.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

static char* osUsage = "./shared_memory_writer pathToFile num0 num1 num2 ...";

void* osCreateMemoryBlock(const char* filePath, unsigned size);

int main(int argc, char** argv) {
	
	check_error(argc >= 3, osUsage);
	
	/* kreira se blok deljene memorije dovoljne velicine za smestanje niza */
	int n = argc - 2;
	int size = (argc-2)*sizeof(int);
	int *niz = osCreateMemoryBlock(argv[1], size);
	
	/* niz brojeva se smesta u deljenu memoriju */
	int i;
	for (i = 0; i<n; i++)
		niz[i] = atoi(argv[i+2]);
		
	/* izmene memorijsko bloka se trajno snimaju na disku, tj.
	 * u fajlu koji predstavlja deljenu memoriju 
	 */
	check_error(munmap(niz, size) != -1, "unmap failed");
	
	exit(EXIT_SUCCESS);
}

void* osCreateMemoryBlock(const char* filePath, unsigned size) {
	
	/* kreira se blok deljene memorije
	 * 
	 * BITNO 1:
	 * kreiranje objekta deljene memorije radi samo jedan proces
	 * ostali procesi samo otvaraju objekat u odgovarajucem modu
	 * 
	 * BITNO 2: 
	 * mod u kome je otvoren fajl deljene memorije
	 * mora biti jednak modu u kome se mapira memorija
	 */ 
	int memFd = shm_open(filePath, O_RDWR | O_CREAT, 0600);
	check_error(memFd != -1, "shm_open failed");
	
	/* velicina fajla se postavlja na zeljenu 
	 * 
	 * BITNO:
	 * ovu operaciju radi samo onaj proces koji kreira objekat
	 * deljene memorije
	 */ 
	check_error(ftruncate(memFd, size) != -1, "ftruncate failed");
	
	void* addr;
	
	/* mapiranje objekta deljene memorije u RAM memoriju
	 * BITNO:
	 * prava pristupa memorijski mapiranom delu moraju da budu jednaka
	 * modu u kome je fajl deljene memorije otvoren
	 */ 
	check_error((addr = mmap(0, size, PROT_READ | PROT_WRITE, \
			MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap failed");
	
	/* zatvara se fajl deljene memorije, jer je njegov sadrzaj u memoriji
	 * i sam fajl na vise nije potreban
	 */ 	
	close(memFd);
	
	/* vracamo adresu memorijski mapiranog bloka */
	return addr;
}
