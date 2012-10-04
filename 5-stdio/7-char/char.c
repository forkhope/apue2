#include <stdio.h>

/* Three functions allow us to read one character at a time.
 * #include <stdio.h>
 * int getc(FILE *fp);
 * int fgetc(FILE *fp);
 * int getchar(void);
 *   All three return: next character if OK, EOF on end of file or error
 * The function getchar() is defined to be equivalent to getc(stdin). The
 * difference between the first two function is getc can be implemented as
 * a macro, whereas fgetc cannot be implemented as a macro.
 *
 * Note that these functions return the same value whether an error occurs
 * or the end of file is reached. To distinguish between the two, we must
 * call either ferror() or feof().
 * #include <stdio.h>
 * int ferror(FILE *fp);
 * int feof(FILE *fp);
 *   Both return: nonzero (true) if condition is true, 0 (false) otherwise
 * void clearerr(FILE *fp);
 * In most implementations, two flags are maintained for each stream in the
 * FILE object: (1) An error flag; (2) An end-of file flag.
 * Both flags are cleared by calling clearerr().
 *
 * We'll find an output function that corresponds to each of the input
 * functions that we've already described.
 * #include <stdio.h>
 * int putc(int c, FILE *fp);
 * int fputc(int c, FILE *fp);
 * int putchar(int c);
 *    All three return: c if OK, EOF on error
 * Like the input functions, putchar(c) is equivalent to putc(c, stdout),
 * and putc can be implemented as a macro, whereas fputc cannot be 
 * implemented as a macro.
 *
 * After reading from a stream, we can push back characters by calling
 * ungetc().
 * #include <stdio.h>
 * int ungetc(int c, FILE *fp);
 *    Returns: c if 0K, EOF on error.
 * The characters that are pushed back are returned by subsequent reads on
 * the steam in reverse order of their pushing.
 */
int main(void)
{
	int c;

	while ((c = getc(stdin)) != EOF)
		putc(c, stdout);

	if (feof(stdin))
		printf("Encounter EOF\n");
	if (ferror(stdin))
		printf("Encounter error\n");
	printf("=============== getc, putc ====\n");

	while ((c = getchar()) != EOF)
		putchar(c);

	if (feof(stdin))
		printf("Encounter EOF\n");
	if (ferror(stdin))
		printf("Encounter error\n");
	printf("=============== getchar, putchar ====\n");

	while ((c = fgetc(stdin)) != EOF)
		fputc(c, stdout);

	if (feof(stdin))
		printf("Encounter EOF\n");
	if (ferror(stdin))
		printf("Encounter error\n");
	printf("=============== fgetc, fputc ====\n");

	c = getchar();
	putchar(c);
	ungetc(c, stdin); 	// 将这个字符重新放回 stdin
	c = getchar();		// 再次从 stdin 读取字符,将会读出刚才的字符
	putchar(c);
	putchar('\n');

	return 0;
}
