#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <utime.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* Three time fields are maintained for each file.
 * -----------------------------------------------------------------------
 * |  Field  |      Description                   |  Example   |  ls     |
 * -----------------------------------------------------------------------
 * |st_atime |last-access time of file data       |read        | -u      |
 * |st_mtime |last-modification time of file data |write       | default |
 * |st_ctime |last-change time of i-node status   |chmod,chown | -c      |
 * -----------------------------------------------------------------------
 * Note the difference between the modification time (st_mtime) and the
 * changed-status time (st_ctime). The modification time is when the
 * contents of the file were last modified. The changed-status time is when
 * the i-node of the file was last modified.
 * Note that the system does not maintain the last-access time for an i-node
 * This is why the functions access() and stat(), for example, don't change
 * any of the three times.
 *
 * The access time and the modification tiem of a file can be changed
 * with the utime() function.
 * #include <utime.h>
 * int utime(const char *pathname, const struct utimbuf *times);
 * 		Returns: 0 if OK, -1 on error.
 * The structure used by this function is
 * 		struct utimbuf {
 * 			time_t actime;		// access time
 * 			time_t modtime;		// modification time
 * 		}
 * utime()函数用于改变文件的访问时间和修改时间,它不能获取这两个时间,如果想
 * 要知道这两个时间,可以使用stat()函数,这两个时间分别对应struct stat结构体
 * 的两个成员: st_atime 和 st_mtime. 通过这两个成员即可知道这两个时间.
 *
 * The operation of this function, and the privileges required to execute
 * it, depend on whether the times argument is NULL.
 * $ If times is a null pointer, the access time and the modification time
 * are both set to the current time. To do this, either the effective user
 * ID of the process must equal the owner ID of the file, or the process
 * must have write permission for the file. 参见 23-utime/目录 中的例子
 * $ If times is a non-null pointer, the access time and the modification
 * time are set to the values in the structure pointed to by times. For
 * this case, the effective user ID of the process must equal the owner ID
 * of the file, or the process must be a superuser process. Merely having
 * write permission for the file is not adequate.
 *
 * Note that we are unable to specify a value for the changed-status time,
 * st_ctime--the time the i-node was last changed--as this filed is
 * automatically updated when the utime() function is called.
 * st_ctime 对应 i-node 的内容被改变的时间, utime()函数会改变 st_atime 和
 * st_mtime,而两个值正好存放在 i-node 里面,所以 st_ctime 的值会随之改变.
 */
int main(int argc, char *argv[])
{
	int i, fd;
	struct stat stbuf;
	struct utimbuf timebuf;

	for (i = 1; i < argc; ++i) {
		if (stat(argv[i], &stbuf) < 0) { /* fetch current times */
			printf("stat %s error: %s\n", argv[i], strerror(errno));
			continue;
		}

		if ((fd = open(argv[i], O_RDWR | O_TRUNC)) < 0) { /* truncate */
			printf("open %s error: %s\n", argv[i], strerror(errno));
			continue;
		}
		close(fd);		/* fd 变量的作用就在于此 */

		timebuf.actime = stbuf.st_atime;
		timebuf.modtime = stbuf.st_mtime;
		if (utime(argv[i], &timebuf) < 0) {	/* reset times */
			printf("utime %s error: %s\n", argv[i], strerror(errno));
			continue;
		}
	}
	return 0;
}
