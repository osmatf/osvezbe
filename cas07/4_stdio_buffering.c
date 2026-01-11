/* Pod baferisanjem se podrazumeva definisanje onoga sto vrsi okidanje fizicke IO operacije.
 * Nebaferisani IO znaci da je okidac za IO operacije sama operacija citanja ili pisanja. (default
 * za stderr) Linijski baferisani IO znaci da je okidac za IO operacije karakter za novi red.
 * (default za terminale) Potpuno baferisani IO znaci da je okidac za IO operacije napunjenost
 * bafera. Fizicka IO operacija se vrsi samo onda kada se stigne do kraja bafera. (default za
 * regularne fajlove)
 *
 *
 * Mod baferisanja moze da se kontrolise pomocu funkcije setvbuf:
 * 1) setvbuf(stdout, NULL, _IONBF, 0); -> magic message ide na stdout [nebaferisani IO]
 * 2) setvbuf(stdout, NULL, _IOLBF, 0); -> magic message ide u fajl [linijski baferisani IO]
 * 3) setvbuf(stdout, NULL, _IOFBF, 0); -> magic message ide u fajl [potpuno baferisani IO]
 * Ako se doda novi red na kraj magic message, onda ce ova poruka ici u fajl samo u slucaju 3.
 */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define check_error(cond, msg)                                                                     \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            perror(msg);                                                                           \
            fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);   \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

static const char *os_Usage = "Usage: ./stdio_buffering path/to/reg/file";

int main(int argc, char **argv)
{
    check_error(2 == argc, os_Usage);

    check_error(setvbuf(stdout, NULL, _IOLBF, 0) == 0, "setvbuf");
    /*
     * Poruka nema novi red ('\n'), pa se u režimu linijskog baferisanja
     * NE ispisuje odmah, već ostaje u internom baferu stdout-a.
     * U ovom trenutku još uvek je vezana za buduće odredište stdout-a.
     */
    const char *magicMessage = "Magic happens to this message!";
    printf("%s", magicMessage);
    /*
     * Otvaramo fajl na koji cemo preusmeriti stdout
     */
    int fd = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, 0644);
    check_error(-1 != fd, "Opening file failed");

    /*
     * Pomoću funkcije dup pravimo kopiju fajl deskriptora za stdout.
     * Ovo radimo kako bismo kasnije mogli da vratimo stdout na terminal.
     */
    int fdCpy = dup(STDOUT_FILENO);
    check_error(-1 != fdCpy, "Copying stdout fd failed");

    /*
     * Preusmeravamo stdout na fajl.
     * Od ovog trenutka svi podaci koji se isprazne iz stdout bafera
     * (automatski ili pozivom fflush) biće upisani u fajl.
     *
     * BITNO:
     * dup2 menja fajl deskriptor, ali NE menja stanje stdio bafera.
     */
    check_error(dup2(fd, STDOUT_FILENO) != -1, "Redirecting stdout to file failed");

    /*
     * Ova poruka sadrži novi red, što u režimu linijskog baferisanja
     * izaziva pražnjenje stdout bafera.
     *
     * Kao rezultat, i prethodna poruka ("magicMessage"), koja je
     * bila zadržana u baferu, sada se zajedno sa ovom porukom
     * upisuje u fajl.
     */
    printf("This message will go to file along with magic message!\n");

    /*
     * Funkcija fflush se koristi za ručno pražnjenje bafera.
     * Argument NULL znači da se prazne svi stdio baferi.
     * U slučaju potpunog baferisanja, fflush je neophodan jer
     * bafer neće biti ispražnjen znakom '\n')
     */
    // fflush(NULL);

    /*
     * Vracamo se na pocetno stanje, tj. terminal ponovo postaje stdout
     * BITNO:
     * obratiti paznju na upotrebu dup2
     */
    check_error(-1 != dup2(fdCpy, STDOUT_FILENO), "Restoring stdout failed");

    /*
     * Stampamo poruku na stdout. (terminal)
     */
    printf("Finally stdout!\n");

    /* zatvaramo fajl deskriptor */
    close(fd);

    return 0;
}
