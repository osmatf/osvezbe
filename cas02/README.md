# Čas 2
Čas pokriva:

- rad sa informacijama o korisnicima i grupama
- rad sa informacijama o fajlovima na sistemu

TLPI poglavlja:

- 8.1, 8.2, 8.3, 8.4
- 15.1, 15.4, 15.4.1 - 15.4.3

Korisno:
- detaljnije o sardžaju `passwd` i `group` fajlova se može naći komandama `man 5 passwd` i `man 5 group`. U odeljku `see also` se može videti spisak svih funkcija koji su u vezi sa pomenutim man stranama.
- spisak makroa za proveru tipa fajla, kao i spisak makroa za proveru dozvola nad fajlom se mogu naći komandom `man 7 inode`. U istoj sekciji nalaze se i dodatne informacije o poljima `stat` strukture.
- komandom `touch` se kreira fajl ukoliko ne postoji, a ukoliko postoji azuriraju mu se vremena pristupa i modifikacije. Više u `man 1 touch`.
- `chmod u-r 1.txt` oduzima pravo čitanja korisniku nad fajlom `1.txt`. Na sličan način se mogu menjati ostala prava grupama i ostalim korisnicima. Više u `man 1 chmod`.
