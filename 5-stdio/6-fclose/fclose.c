#include <stdio.h>

/* An open stream is closed by calling fclose.
 * #include <stdio.h>
 * int fclose(FILE *fp);
 * 		Returns: 0 if OK, EOF on error
 *
 * 	When a process terminates normally, either by calling the exit()
 * 	function directly or by returning from the main() function, all
 * 	standard I/O streams with unwritten buffered data are flushed, and all
 * 	open standard I/O streams are closed.
 */
int main(void)
{
	FILE *fp;

	if ((fp = fopen("tian", "w")) == NULL)
		printf("fopen w tian error: %m\n");
	fputs("tianxiayouqingren\n", fp);

	fclose(fp);
	return 0;
}
