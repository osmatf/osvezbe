# Pitalice sa pokazivacima

Kratke pitalice za demonstraciju rada sa pokazivacima.

---

## Uvod: pokazivaci u C-u

Svaka promenljiva se nalazi negde u memoriji i ima svoju **adresu**. Pokazivac je promenljiva cija je vrednost adresa neke druge promenljive.

```c
int x = 5;
int *p = &x;          // p cuva adresu promenljive x

printf("%d\n", *p);   // 5
*p = 8;               // menja x preko pokazivaca
printf("%d\n", x);    // 8
```

- `int *p` deklarise pokazivac na `int`. Tip kaze sta se nalazi na toj adresi, tj. kako da se protumaci vrednost na koju `p` pokazuje.
- `&x` daje adresu promenljive `x`.
- `*p` je vrednost na adresi koju cuva `p` (dereferenciranje). Preko `*p` moze i da se cita i da se upisuje.
- Znak `*` u deklaraciji znaci "pokazivac", a u izrazu znaci "vrednost na adresi".
- `NULL` je posebna vrednost koja znaci da pokazivac ne pokazuje ni na sta. Dereferenciranje `NULL` pokazivaca obicno obori program (*Segmentation fault*).
- Adresa moze da se ispise sa `printf("%p\n", (void *)p);`.

---

## Pitalica 1: Zagrevanje

Sta ce se ispisati?

```c
int a = 3, b = 7;
int *p = &a;
*p = 10;
p = &b;
*p += 1;
printf("%d %d\n", a, b);
```

<details>
<summary>Resenje</summary>

```
10 8
```

- `p` prvo pokazuje na `a`, pa `*p = 10` menja `a` u 10.
- `p = &b` preusmerava pokazivac na `b` (sam `a` se ne menja).
- `*p += 1` sada menja `b`, pa `b` postaje 8.

</details>

---

## Pitalica 2: Dva pokazivaca na istu promenljivu

Sta ce se ispisati?

```c
int x = 1;
int *p = &x, *q = &x;
*p = 5;
*q = *q * 2;
printf("%d\n", x);
```

<details>
<summary>Resenje</summary>

```
10
```

I `p` i `q` cuvaju istu adresu (adresu promenljive `x`). Zato `*p = 5` postavlja `x` na 5,
a `*q = *q * 2` cita tu istu vrednost (5) i upisuje 10 nazad u `x`.

</details>

---

## Pitalica 3: Swap - koji radi?

Sta ce se ispisati?

```c
void swap1(int a, int b)   { int t = a;  a = b;   b = t; }
void swap2(int *a, int *b) { int t = *a; *a = *b; *b = t; }

int x = 1, y = 2;
swap1(x, y);   printf("%d %d\n", x, y);
swap2(&x, &y); printf("%d %d\n", x, y);
```

<details>
<summary>Resenje</summary>

```
1 2
2 1
```

- U C-u se argumenti prenose **po vrednosti**. `swap1` dobija kopije od `x` i `y` i menja samo te
  kopije, a originali ostaju isti.
- `swap2` dobija **adrese** promenljivih `x` i `y`, pa preko `*a` i `*b` menja bas originale.

</details>

---

## Pitalica 4: Pokazivaci i nizovi

Sta ce se ispisati?

```c
int a[] = {10, 20, 30, 40};
int *p = a;
printf("%d %d %d\n", *p, *(p + 2), p[3]);
p++;
printf("%d %d\n", *p, p - a);
```

<details>
<summary>Resenje</summary>

```
10 30 40
20 1
```

- Ime niza `a` se ponasa kao adresa prvog elementa, pa `p = a` znaci `p = &a[0]`.
- `*(p + 2)` i `p[2]` su isto. `p + 2` se pomera za 2 **elementa** (ne bajta), tj. za `2 * sizeof(int)` bajtova.
- Posle `p++` pokazivac `p` pokazuje na `a[1]`, a `p - a` je razlika u broju elemenata, tj. 1.

</details>

---

## Pitalica 5: Prioritet operatora - `*p++` / `(*p)++` / `++*p`

Sta ce se ispisati?

```c
int a[] = {1, 5, 9};
int *p = a;
int x = *p++;
int y = (*p)++;
int z = ++*p;
printf("%d %d %d | %d %d %d\n", x, y, z, a[0], a[1], a[2]);
```

<details>
<summary>Resenje</summary>

```
1 5 7 | 1 7 9
```

- `*p++` je isto sto i `*(p++)`: procita `a[0]` (1), pa pomeri **pokazivac** na `a[1]`. Dakle `x = 1`.
- `(*p)++` uvecava **vrednost** na koju `p` pokazuje (postfiks): vrati staru vrednost 5, a `a[1]` postaje 6. Dakle `y = 5`.
- `++*p` je isto sto i `++(*p)`: prvo uveca `a[1]` na 7, pa vrati 7. Dakle `z = 7`.
- Niz se promenio samo na poziciji 1.

</details>

---

## Pitalica 6: Pokazivac na pokazivac

Sta ce se ispisati?

```c
int x = 5, y = 9;
int *p = &x;
int **pp = &p;
**pp = 7;
*pp = &y;
*p = 1;
printf("%d %d\n", x, y);
```

<details>
<summary>Resenje</summary>

```
7 1
```

- `pp` pokazuje na `p`, a `p` pokazuje na `x`.
- `**pp` je `*p`, sto je `x`, pa `**pp = 7` postavlja `x` na 7.
- `*pp` je sam `p`, pa `*pp = &y` preusmerava `p` da pokazuje na `y`.
- Zato `*p = 1` sada menja `y`.

</details>

---

## Pitalica 7: `sizeof` niza i `sizeof` pokazivaca

Sta ce se ispisati?

```c
void f(int a[]) { printf("%lu\n", sizeof(a)); }

int a[10];
printf("%lu\n", sizeof(a));
f(a);
```

<details>
<summary>Resenje</summary>

```
40
8
```

(na 64-bitnom sistemu gde je `int` 4 bajta)

- U `main`-u je `a` pravi niz od 10 `int`-ova, pa je `sizeof(a) = 10 * 4 = 40`.
- Kada se niz prosledi funkciji, on se "raspada" (*array decay*) u pokazivac na prvi element.
  Parametar `int a[]` je u stvari `int *a`, pa je `sizeof(a)` velicina pokazivaca, tj. 8.
- Zbog ovoga se funkcijama koje rade sa nizovima uvek prosledjuje i duzina niza.
- `gcc -Wall` ovde daje upozorenje `-Wsizeof-array-argument`.

</details>

---

## Primer: `malloc` - dinamicka alokacija

```c
int n = 5;
int *a = malloc(n * sizeof(int));
if (a == NULL) {
    printf("Nema dovoljno memorije\n");
    return 1;
}

for (int i = 0; i < n; i++)
    a[i] = i * i;

for (int i = 0; i < n; i++)
    printf("%d ", a[i]);   // 0 1 4 9 16
printf("\n");

free(a);
```

- `malloc(k)` iz `<stdlib.h>` rezervise `k` **bajtova** na hipu (*heap*) i vraca adresu pocetka tog bloka.
  Zato se broj elemenata mnozi sa `sizeof(int)`.
- Za razliku od lokalnih promenljivih (stek), ova memorija ne nestaje kad se funkcija zavrsi, vec zivi dok je ne oslobodimo sa `free`.
- Velicina moze da se odredi tek u toku izvrsavanja (npr. `n` ucitano sa ulaza).
- Dobijeni pokazivac se koristi kao niz: `a[i]` je isto sto i `*(a + i)` (pitalica 4).
- Ako nema dovoljno memorije, `malloc` vraca `NULL`, pa rezultat treba proveriti.
- Dobijena memorija **nije inicijalizovana** (sadrzi smece). Ako treba da bude popunjena nulama, koristi se `calloc`.
- Svaki `malloc` treba da ima svoj `free`, inace curi memorija (*memory leak*). Posle `free` pokazivac vise ne sme da se koristi.

---

## Pitalica 8: Nadji gresku

Sta nije u redu sa ovom funkcijom?

```c
int *napravi(void) {
    int x = 42;
    return &x;
}

int *p = napravi();
printf("%d\n", *p);
```

<details>
<summary>Resenje</summary>

Funkcija vraca adresu **lokalne** promenljive. `x` zivi na steku samo dok se `napravi` izvrsava.
Cim se funkcija zavrsi, ta memorija vise ne pripada `x`, pa je `*p` **nedefinisano ponasanje**:
moze da ispise 42, smece ili da pukne.

Sa `gcc` program obicno pukne (*Segmentation fault*), jer gcc, posto zna da je ovo greska, umesto adrese vrati `NULL`.
Kompajler i upozorava: `function returns address of local variable [-Wreturn-local-addr]`.

Ispravke:

```c
// 1) dinamicka alokacija (pozivalac radi free)
int *napravi(void) {
    int *x = malloc(sizeof(int));
    *x = 42;
    return x;
}

// 2) static promenljiva (zivi dok traje program)
int *napravi(void) {
    static int x = 42;
    return &x;
}
```

</details>

---

## Pitalica 9: Sta pukne?

Koja od dve dodele ce napraviti problem?

```c
char s[] = "abc";
char *t  = "abc";
s[0] = 'X';
t[0] = 'X';
```

<details>
<summary>Resenje</summary>

Puca `t[0] = 'X'`.

- `char s[] = "abc"` pravi **niz** na steku i u njega kopira znakove `'a', 'b', 'c', '\0'`. Taj niz je nas i sme da se menja.
- `char *t = "abc"` pravi samo **pokazivac** na string literal, koji se nalazi u read-only memoriji.
  Pokusaj upisa je nedefinisano ponasanje i u praksi daje *Segmentation fault*.

Zato se za string literale pise `const char *t = "abc";`. Tada kompajler prijavi gresku vec pri prevodjenju.

</details>

---

## Pitalica 10: Zasto ovo puca?

```c
void alociraj(int *p) {
    p = malloc(sizeof(int));
    *p = 5;
}

int *q = NULL;
alociraj(q);
printf("%d\n", *q);
```

<details>
<summary>Resenje</summary>

Isti problem kao kod `swap1` iz pitalice 3, samo sa pokazivacem. Pokazivac `q` se prenosi **po vrednosti**:
`p` je kopija od `q`. Funkcija menja kopiju (`p = malloc(...)`), a `q` u `main`-u ostaje `NULL`.
Posle toga `*q` dereferencira `NULL`, pa program pukne (*Segmentation fault*).
Usput curi i memorija, jer adresu dobijenu od `malloc` niko vise nema.

Da bi funkcija promenila sam pokazivac, mora da dobije **adresu pokazivaca**, tj. `int **`:

```c
void alociraj(int **p) {
    *p = malloc(sizeof(int));
    **p = 5;
}

int *q = NULL;
alociraj(&q);
printf("%d\n", *q);   // 5
free(q);
```

Alternativa je da funkcija vrati novi pokazivac: `int *alociraj(void)`.

</details>

---

## Pitalica 11: `void *` - ista adresa, razlicito tumacenje

Sta ce se ispisati?

```c
int x = 65;
void *v = &x;

// printf("%d\n", *v);   // ?

printf("%d\n", *(int *)v);
printf("%c\n", *(char *)v);
printf("%d\n", (int *)v == &x);
```

<details>
<summary>Resenje</summary>

```
65
A
1
```

- `void *` je "genericki" pokazivac: cuva adresu, ali **ne zna tip** podatka na toj adresi.
  Zato `*v` ne moze direktno da se dereferencira (`error: invalid use of void expression`), jer kompajler ne zna koliko bajtova da procita ni kako da ih protumaci.
  Pre dereferenciranja mora da se kastuje u konkretan tip.
- `*(int *)v` cita 4 bajta kao `int`, pa dobijamo 65.
- `*(char *)v` cita **samo prvi bajt** iste adrese kao `char`. Na little-endian masini (x86, ARM) prvi bajt je najnizi bajt broja, tj. 65, sto je `'A'` u ASCII tabeli.
  Na big-endian masini bi se procitao bajt 0.
- Kastovanje ne menja adresu: `(int *)v == &x` je tacno (1).
- U C-u svaki pokazivac na objekat moze da se dodeli `void *` i nazad **bez** eksplicitnog kastovanja.
  Zato `malloc` (koji vraca `void *`) moze da se pise kao `int *p = malloc(...)`.

</details>

---

## Pitalica 12: `memcpy` - kopiranje bajtova

Sta ce se ispisati?

```c
int a[] = {1, 2, 3, 4, 5};
int b[5] = {0};
int c[5] = {0};

// b = a;   // ?

memcpy(b, a, sizeof(a));
memcpy(c, a, 2 * sizeof(int));
memcpy(c + 2, a + 3, 2 * sizeof(int));

// ispis nizova b i c
```

<details>
<summary>Resenje</summary>

```
1 2 3 4 5
1 2 4 5 0
```

- Nizovi u C-u ne mogu da se dodeljuju: `b = a` se ne kompajlira (`error: assignment to expression with array type`).
  Za kopiranje se koristi `memcpy` iz `<string.h>`:

  ```c
  void *memcpy(void *dest, const void *src, size_t n);
  ```

  Kopira **tacno `n` bajtova** sa adrese `src` na adresu `dest` i vraca `dest`.
  Parametri su `void *`, pa `memcpy` radi sa podacima bilo kog tipa: ne zna sta kopira, samo koliko bajtova.
- `memcpy(b, a, sizeof(a))`: `sizeof(a)` je `5 * 4 = 20` bajtova, pa se kopira ceo niz.
- `memcpy(c, a, 2 * sizeof(int))`: treci argument je broj **bajtova**, a ne elemenata, pa se broj elemenata mnozi sa `sizeof(int)`.
  Kopiraju se `a[0]` i `a[1]` u `c[0]` i `c[1]`. Cesta greska je `memcpy(c, a, 2)`, sto kopira samo 2 bajta, a ne 2 elementa.
- `memcpy(c + 2, a + 3, 2 * sizeof(int))`: kopira deo niza. Uz pokazivacku aritmetiku iz pitalice 4,
  `a + 3` je adresa `a[3]`, a `c + 2` adresa `c[2]`, pa se `a[3]` i `a[4]` (4 i 5) kopiraju u `c[2]` i `c[3]`.
- `memcpy` ne proverava granice: ako je `n` vece od odredista, pise se van niza.
- Ako se izvor i odrediste preklapaju, `memcpy` ima nedefinisano ponasanje. Tada se koristi `memmove`, koji radi ispravno i sa preklapanjem.

</details>

---

## Pitalica 13: Genericki swap sa `void *`

Sta ce se ispisati? Zasto funkcija mora da dobije i velicinu `n`?

```c
void swap(void *a, void *b, size_t n) {
    char *tmp = malloc(n);
    if (tmp == NULL)
        exit(EXIT_FAILURE);

    memcpy(tmp, a, n);
    memcpy(a, b, n);
    memcpy(b, tmp, n);
    free(tmp);
}

int x = 1, y = 2;
double d = 1.5, e = 2.5;
char s[] = "abc", t[] = "xyz";

swap(&x, &y, sizeof(int));
swap(&d, &e, sizeof(double));
swap(s, t, sizeof(s));

printf("%d %d\n", x, y);
printf("%.1f %.1f\n", d, e);
printf("%s %s\n", s, t);
```

<details>
<summary>Resenje</summary>

```
2 1
2.5 1.5
xyz abc
```

- Ista funkcija menja vrednosti bilo kog tipa, jer `void *` prima adresu bilo cega.
- Posto `void *` ne nosi informaciju o tipu, funkcija **ne zna koliko je podatak veliki**.
  Zato joj se velicina prosledjuje kao `n`, a bajtovi se kopiraju sa `memcpy` iz pitalice 12 (koji takodje radi sa `void *`).
- Velicina pomocnog bafera `tmp` se zna tek u toku izvrsavanja, pa se on alocira sa `malloc(n)` (primer pre pitalice 8).
  `n` je vec broj bajtova, a `char` je 1 bajt, pa ne treba mnoziti sa `sizeof`. Na kraju se bafer oslobadja sa `free`.
- Za razliku od `swap2` iz pitalice 3, ovde nema `*a = *b`, jer `void *` ne moze da se dereferencira.
- Istu ideju koristi standardni `qsort`: dobija niz kao `void *`, velicinu elementa i funkciju za poredjenje koja prima `const void *`.

</details>
