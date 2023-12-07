/*
Primer definisanja makroa u vise redova 
(znak '\' omogucava pisanje u vise redova).
*/

#include <stdio.h>
#include <stdlib.h>

/*
#define proizvod(a,b)\
		int rez=((a)*(b));\
		printf("%d\n",rez);
*/
/*
Zasto do while? 

Makro se tada ponasa slicno kao funkcijski poziv ili
naredba jezika, odnosno zahteva ';' na kraju, pri cemu
i dalje nema kreiranja stek okvira kao u slucaju 
funkcijskih poziva.
*/
#define proizvod(a,b)\
	do {\
		int rez=((a)*(b));\
		printf("%d\n",rez);\
	} while (0)


#define proizvod_bez_do_while(a,b)\
	{\
		int rez=((a)*(b));\
		printf("%d\n",rez);\
	}


int main(int argc, char** argv) {
	
	int x = 10;
	int y = 5;

	//int rez = proizvod(x+y,x-y);
	//int rez = ((x+y)*(x-y)); zbog ovoga moraju da stoje zagrade u makrou
	
	//printf("%d\n", rez);
	
	//bez do while ograde, makro proizvod uzrokuje gresku
	if (x > 5) 
		proizvod(x,y);

	/* ukoliko bismo stavili samo zagrade u definiciji makroa (bez do while ograde)
	   else deo bi "visio"
	*/
	/*
	if (x > 5)
		proizvod_bez_do_while(x,y);
	else
		printf("Greska\n");
	*/

	exit(EXIT_SUCCESS);
}
