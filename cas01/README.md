# Čas 1
Čas pokriva:

- obnavljanje potrebnog gradiva iz programiranja 2
- uvođenje makroa za proveru grešaka
- kodove grešaka i primer korišćenja promenljive errno

TLPI poglavlja:

- 3.4 - Handling Errors from System Calls and Library Functions

Korisno:

Prva faza kompajliranja C programa je preprocesiranje. Preprocesor obrađuje direktive koje počinju znakom #, kao što su `#include`, `#define`, `#ifdef` i sl.

Opcija -E u GCC-u zaustavlja proces prevođenja nakon faze preprocesiranja i ispisuje rezultat na standardni izlaz:

`gcc -E program.c`

Dobijeni izlaz predstavlja C kod u kome su:
- zamenjeni svi makroi (#define)
- ubačen sadržaj svih uključenih zaglavlja (#include)
- uklonjen ili zadržan kod u zavisnosti od uslovne kompilacije (#if, #ifdef, #ifndef)
