#include <stdio.h>

int main(int argc, char *argv[])
{
	int i;
	char **p;
	extern char **environ;

	for (i = 0; i < argc; ++i)	/* echo all command-line args */
		printf("argv[%d]: %s\n", i, argv[i]);

	for (p = environ; *p != NULL; ++p)	/* and all env strings */
		printf("%s\n", *p);

	return 0;
}
