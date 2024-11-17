/*
Poenta ovog primera je da prikaze gde se u virtuelnoj memoriji
procesa nalaze promenljive i memorija na koju one pokazuju.

BITNO: slika 6-1 u TLPI u poglavlju 6.4.
*/
#include <stdio.h>
#include <stdlib.h>

int niz_intova[1000]; /* neinicijalizovani deo segmenta podataka */
int niz_brojeva[] = {1, 2, 3, 4, 5}; /* inicijalizovani deo segmenta podataka */

/* isto kao niz_intova, ali je niz_intova2 vidljiv samo unutar ovog fajla.
 * Ako program kompajliramo iz vise datoteka, u drugim datotekama necemo
 * moci da vidimo ovaj niz_intova2.
 */
static int niz_intova2[1000];

static int *q; /* promenljiva p je vidljiva samo u ovom fajlu i nalazi se u neinicijalizovanom delu segmenta podataka */

/* static ispred imena funkcije oznacava da ce funkcija biti 
 * vidljiva samo u fajlu u kom je definisana. Dakle, ako 
 * program kompajliramo iz vise datoteka, druge datoteke nece moci
 * da pozovu ovu funkciju */
static int stampaj(int x){ /* x se nalazi u stek okviru funkcije stampaj*/
    printf("%d\n", x);

    return x;
}

static int stampaj_kvadrat(int x){/* x se nalazi u stek okviru funkcije stampaj*/
    int res; /* res se nalazi u stek okviru funkcije stampaj*/

    res = x * x;
    printf("%d\n", res);

    return res; /* povratna vrednost se vraca kroz registar procesora */
}

int main(int argc, char** argv) {
	static int broj = 5; /* promenljiva broj se nalazi u inicijalizovanom delu segmenta podatakai vidljiva je samo unutar funkcije main */

    static int brojevi[100]; /* niz brojevi se nalazi u neinicijalizovanom delu segmenta podataka i vidljiv je samo unutar funkcije main */
	char *p; /* promenljiva p se nalazi u stek okviru funkcije main */

    p = malloc(1024); /* p pokazuje na 1024 bajta koji se nalaze na hipu */

    q = malloc(1024); /* q je i dalje u segmentu podataka, ali 1024 bajta na koje q pokazuje se nalazi na hipu. */

    free(p);
    free(q);
	exit(EXIT_SUCCESS);
}
