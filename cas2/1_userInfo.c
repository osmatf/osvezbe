/*
Program treba da ispise informacije o korisniku cije je ime 'username',
pri cemu se informacije dobijaju iz passwd fajla.

Poziv programa:
./userInfo username
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* neophodno zaglavlje prilikom rada sa funkcijama 
 * koje citaju passwd fajl
 * 
 * man 5 passwd za detelje o passwd fajlu
 */
#include <pwd.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

static char* osUsage = "./userInfo username";

/* funkcija ispisuje informacije o korisniku iz passwd fajla */
bool os_get_user_info(const char* username, FILE* outStream) {
	
	/* citamo informacije iz passwd fajla o korisniku sa
	 * korisnickim imenom username
	 * funkcija getpwnam vraca pokazivac na staticki alociranu strukturu
	 * TLPI za detalje
	 * 
	 * BITNO: strukturu nikada ne smete eksplicitno da oslobadjate
	 */ 
	struct passwd* userInfo = getpwnam(username);
	/* u slucaju greske ili nepostojanja podataka u passwd fajlu o 
	 * korisniku sa imenom username funkcija getpwnam vraca NULL
	 * i postavlja errno na odgovarajucu vrednost 
	 */
	if (userInfo == NULL)
		return false;
	
	/* ispisujemo informacije koje postoje u passwd  fajlu korisniku */
	fprintf(outStream, "User name: %s\n", userInfo->pw_name);
	fprintf(outStream, "Numerical user id: %jd\n", (intmax_t)userInfo->pw_uid);
	fprintf(outStream, "Numerical group id: %jd\n", (intmax_t)userInfo->pw_gid);
	fprintf(outStream, "Comment: %s\n", userInfo->pw_gecos);
	fprintf(outStream, "Home directory: %s\n", userInfo->pw_dir);
	fprintf(outStream, "Shell: %s\n", userInfo->pw_shell);
	
	return true;
}

int main(int argc, char** argv) {
	
	check_error(argc == 2, osUsage);
	check_error(os_get_user_info(argv[1], stdout), "get user info failed");
	
	exit(EXIT_SUCCESS);
}
