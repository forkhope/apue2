#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

/* Any process can find out its real and effective user ID and group ID.
 * Sometimes, however, we want to find out the login name of the user who's
 * running the program. We can call getpwuid(getuid()), or the getlogin()
 * function. The getlogin() function provides a way to fetch that login name
 * #include <unistd.h>
 * char *getlogin(void);
 * 		Returns: pointer to string giving login name if OK, NULL on error.
 *
 * This function can fail if process is not attached to a terminal that a
 * user logged in to. We normally call these processes daemons.
 *
 * The environment variable LOGNAME is usually initialized with the user's
 * login name by login(1) and inherited by the login shell. Realize, however
 * that a user can modify an environment variable, so we shouldn't use
 * LOGNAME to validate the user in any way. Instead, getlogin() should be
 * used
 */
int main(void)
{
	char *p;

	if ((p = getlogin()) == NULL) {
		printf("getlogin error: %s\n", strerror(errno));
		return 1;
	}
	printf("getlong(): %s\n", p);

	if ((p = getenv("LOGNAME")) != NULL)
		printf("LOGNAME = %s\n", p);
	return 0;
}
