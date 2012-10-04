#include <stdlib.h>
#include <stdio.h>

/* ISO C defines a function that we can use to fetch values from the
 * environment, but this standard says that the contents of the environment
 * are implementation defined.
 * #include <stdlib.h>
 * char *getenv(const char *name);
 * 		Returns: pointer to value associated with name, NULL if not found
 * Note that this function returns a pointer to the value of a name=value
 * string. We should always use getenv to fetch a specific value from the
 * environment, instead of accessing environ directly.
 */
int main(void)
{
	printf("COLUMNS: terminal width: %s\n", getenv("COLUMNS"));
	printf("DATEMSK: getdate template file pathname: %s\n", 
			getenv("DATEMSK"));
	printf("HOME: home directory: %s\n", getenv("HOME"));
	printf("LANG: name of locale: %s\n", getenv("LANG"));
	printf("LC_ALL: name of locale: %s\n", getenv("LC_ALL"));
	printf("LC_COLLATE: name of locale for collation: %s\n", 
			getenv("LC_COLLATE"));
	printf("LC_CTYPE: name of locale for character classification: %s\n",
			getenv("LC_CTYPE"));
	printf("LC_MESSAGES: name of locale for messages: %s\n", 
			getenv("LC_MESSAGES"));
	printf("LC_MONETARY: name of locale for monetry editing: %s\n",
			getenv("LC_MONETARY"));
	printf("LC_NUMERIC: name of locale for numeric editing: %s\n",
			getenv("LC_NUMERIC"));
	printf("LC_TIME: name of locale for date/time formatting: %s\n",
			getenv("LC_TIME"));
	printf("LINES: terminal height: %s\n", getenv("LINES"));
	printf("LOGNAME: login name: %s\n", getenv("LOGNAME"));
	printf("MSGVERB: fmtmsg message components to process: %s\n",
			getenv("MSGVERB"));
	printf("NLSPATH: sequence of templates for message catalogs: %s\n",
			getenv("NLSPATH"));
	printf("PATH: list of path prefixes to search for "
			"executable file: %s\n", getenv("PATH"));
	printf("PWD: absolute pathname of current working directory: %s\n",
			getenv("PWD"));
	printf("SHELL: name of user's preferred shell: %s\n", getenv("SHELL"));
	printf("TERM: terminal type: %s\n", getenv("TERM"));
	printf("TMPDIR: pathname of director for creating temporary files: "
			"%s\n", getenv("TMPDIR"));
	printf("TZ: time zone information: %s\n", getenv("TZ"));

	return 0;
}
