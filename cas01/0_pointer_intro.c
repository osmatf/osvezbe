/*
Uvodni primer sa pokazivacima u C jeziku (isto kao C++).
Pokazivaci su promenljive koje cuvaju adrese drugih promenljivih.
U ovom primeru, pokazivac p cuva adresu promenljive value, sto nam omogucava da
pristupimo vrednosti promenljive value preko pokazivaca.
*/
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int value = 5;

    int *p = &value; // Postavljamo pokazivac na adresu promenljive value

    printf("Vrednost promenljive value: %d\n", value);
    printf("Adresa promenljive value: %p\n", &value);
    printf("Vrednost pokazivaca p: %p\n", p);
    printf("Vrednost na koju pokazivac p pokazuje: %d\n", *p);

    *p = 10; // Menjamo vrednost promenljive value preko pokazivaca

    // Sada je vrednost promenljive value 10 iako nismo direktno uradili nista sa promenljivom value
    printf("Vrednost promenljive value: %d\n", value);
    printf("Adresa promenljive value: %p\n", &value);
    printf("Vrednost pokazivaca p: %p\n", p);
    printf("Vrednost na koju pokazivac p pokazuje: %d\n", *p);

    exit(EXIT_SUCCESS);
}
