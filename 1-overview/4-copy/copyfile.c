#include "apue.h"

int main(void)
{
	int c;

	while ((c = getc(stdin)) != EOF)
		if (putc(c, stdout) == EOF)
			err_sys("copyfile: error when putc to stdout");
	if (ferror(stdin))
		err_sys("copyfile: error when getc from stdin");
	exit(0);
}
