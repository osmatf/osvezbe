/*
Primer cestih gresaka na ispitu: neinicijalizovani pokazivac
i string bez terminirajuce nule. 
*/
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    
    char* s;
    
    char t[50];
    
    t[0] = 'a';
    t[1] = 'b';
    t[2] = 'c';
    //t[3] = 0;
    //t[3] = '\0';
    
    /*
    U ovom trenutku vrednost pokazivaca s je nesto sto se naslo u memoriji
    u trenutku pokretanja programa, sto znaci da pokazivac s pokazuje na nepoznatu
    lokaciju. Drugim recima, scanf ce upisati nesto u deo memorije koji moze biti vazan
    za normalno funkcionisanje programa, pa moze dovesti do nezeljenog ponasanja
    */
    //scanf("%s", s);
    
    /*
    Niska t nije terminirana nulom, pa ce potencijalno biti ispisani i neki karakteri nakon 'c',
    u zavisnosti od toga gde se nalazi prva sledeca terminirajuca nula.
    */
    printf("%s\n", t);
    
    exit(EXIT_SUCCESS);
}
