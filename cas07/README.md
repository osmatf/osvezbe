# Čas 7

Čas pokriva:
- slanje i obrada signala
- execv sistemski poziv
- komunikaciju izmedju procesa putem FIFO fajlova

TLPI poglavlja:
- 20, 20.1, 20.2, 20.3, 20.4, 20.5, 20.14
- 21.1.2
- 44.7, 44.8, 44.10

Korisno:
- komandom `echo [string]` se `string` ispisuje na standardni izlaz. U terminalu, simbol `>` se koristi za preuzmeravanje standardnog izlaza. Tako na primer, komandom `echo "abc" > 1.txt` se u fajl `1.txt` upisuje string `abc`. Ovo je korisno pri testiranju programa koji rade sa fifo fajlovima. Ako nas program ocekuje da nesto bude upisano u fifo fajl `fifo_fajl`, a mi hocemo da procitamo sadrzaj, onda iz drugog terminala mozemo uraditi `echo "test_poruka" > fifo_fajl` da bismo lakse testirali (umesto da pisemo ceo program koji upisuje tekst "test_poruka" u trazeni fifo fajl).