#include <stdio.h>

/* There are three ways to position a standard I/O stream:
 * (1) The two functions ftell() and fseek(). They have been around since
 * Version 7, but they assume that a file's position can be stored in a 
 * long integer.
 * (2) The two functions ftello() and fseeko(). They were introduced in the
 * Single UNIX Specification to allow for file offsets that might not fit in
 * a long integter. They replace the long integer with the off_t data type.
 * (3) The two functions fgetpos() and fsetpos(). They were introducted by
 * ISO C. They use an abstract data type, fpos_t, that records a file's
 * position. This data type can be made as big as necessary to record a
 * file's position.
 *
 * #include <stdio.h>
 * long ftell(FILE *fp);
 * 	Returns: current file position indicator if 0K, -1L on error
 * int fseek(FILE *fp, long offset, int whence);
 * 	Returns: 0 if OK, nonzero on error
 * void rewind(FILE *fp);
 *
 * off_t ftello(FILE *fp);
 * 	Returns: current file position indicator if OK, (off_t)-1 on error
 * int fseeko(FILE *fp, off_t offset, int whence);
 *  Returns: 0 if OK, nonzero on error
 *
 * int fgetpos(FILE *restrict fp, fpos_t *restrict pos);
 * int fsetpos(FILE *fp, const fpos_t *pos);
 * 	Both return: 0 if OK, nonzero on error.
 */
int main(void)
{
	FILE *fp;
	long pos_l;

	if ((fp = fopen("Makefile", "r+")) == NULL) {
		printf("fopen r+ Makefile error\n");
		return 1;
	}

	pos_l = ftell(fp);
	printf("pos_l: %ld\n", pos_l);

	return 0;	
}
