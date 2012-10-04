#include <stdlib.h>
#include <stdio.h>

/* In addition to fetching the value of an environment variable, sometimes
 * we may want to set an environment variable. We may want to change the
 * value of an existing variable or add a new variable to the environment.
 * #include <stdlib.h>
 * int putenv(char *str);
 * 		Returns: 0 if OK, nonzero on error
 * int setenv(const char *name, const char *value, int rewrite);
 * int unsetenv(const char *name);
 * 		Both return: 0 if OK, -1 on error.
 */
int main(void)
{
	char *myname = "MYNAME=lixianyi";
	const char *name = "MYNAME";
	const char *value = "lixianyi-lxy";

	/* The putenv() function takes a string of the form name=value and
	 * places it in the environment list. If name already exists, its
	 * old definition is first removed.
	 */
	if (putenv(myname) != 0)
		printf("Can't put %s to environment list\n", myname);	
	printf("getenv MYNAME: %s\n", getenv("MYNAME"));

	/* The setenv() function sets name to value. If name already exists in
	 * the environment, then (a) if rewrite is nonzero, the existing
	 * definition for name is first removed; (b) if rewrite is 0, an
	 * existing definition for name is not removed, name is not set to the
	 * new value, and no error occurs.
	 */
	if (setenv(name, value, 1) != 0)
		printf("Can't set %s to %s\n", name, value);
	printf("getenv MYNAME: %s\n", getenv("MYNAME"));

	if (setenv(name, "LiXianYi", 0) != 0)
		printf("Can't set %s to %s\n", name, "LiXianYi");
	printf("After: getenv MYNAME: %s\n", getenv("MYNAME"));

	/* The unsetenv() function removes and definition of name. It is not
	 * an error if such a definition does not exist.
	 */
	if (unsetenv("MYNAME") != 0)
		printf("Can't unsetenv MYNAME\n");
	printf("getenv MYNAME: %s\n", getenv("MYNAME"));

	if (unsetenv("ABCD") != 0)
		printf("Can't unsetenv ABCD\n");
	else
		printf("unsetenv ABCD\n");

	return 0;
}
