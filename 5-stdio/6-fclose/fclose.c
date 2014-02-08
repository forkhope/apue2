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
 *
 * 	POSIX.1中有这样的描述: The fclose() function shall perform the
 * 	equivalent of a close() on the file descriptor that is associated with
 * 	the stream pointed to by stream. 也就是说,在linux系统下,FILE结构体会
 * 	有一个文件描述符关联实际的文件,调用fclose()函数关闭FILE *类型的指针时,
 * 	其对应的文件描述符也会被关闭.
 * 	当执行 fp = fdopen(fd, "w"); 后,我们就有两个变量关联到同一个文件,fp和fd
 * 	此时既能读写fp,又能读写fd,那么有个问题是,我们调用fclose(fp);关闭fp后,
 * 	是否还需要、或者说是否还能调用close()函数来关闭fd? 由上面的描述可以知道,
 * 	不需要、也不能,因为执行完fclose(fp);后,fd和fp都被关闭了,再执行close(fd);
 * 	会报错: Bad file descriptor.
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
