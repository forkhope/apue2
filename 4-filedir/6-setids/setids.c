#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Set-User-ID and Set-Group-ID
 * Every file has an owner and a group owner. The owner is specified by the
 * st_uid member of the stat structure; the group owner, by the st_gid
 * member.
 * When we execute a program file, the effective user ID of the process is
 * usually the real user ID, and the effective group ID is usually the real
 * group ID. But the capability exists to set a special flag in the file's
 * mode word (st_mode) that says "when this file is executed, set the
 * effective user ID of the process to be the owner of the file (st_uid)."
 * Similarly, another bit can be set in the file's mode word that causes
 * the effective group ID to be the group owner of the file (st_gid). These
 * two bits in the file's mode word are called the set-user-ID bit and the
 * set-group-ID bit.
 * Returning to the stat() function, the set-user-ID bit and the
 * set-group-ID bit are contained in the file's st_mode value. These two
 * bits can be tested against the constants S_ISUID and S_ISGID.
 *
 * 书中提到一个这样的程序: passwd,该命令可以让普通用户改变自己的密码,并将新
 * 密码写入 /etc/passwd 文件中,但是 /etc/passwd 文件属于根用户所有,普通用户
 * 无法写入该文件. passwd 可执行命令文件设置了 set-user-ID 位,而该命令的uid
 * 是根用户,所以执行该命令时,其effective user ID是根用户,可写入/etc/passwd.
 * 注意,这里是说 passwd 这个命令被设置了 ser-user-ID 位,不是说 /etc/passwd
 * 文件被设置了 set-user-ID 位. 下面的程序用于判断文件是否设置了set-user-ID
 * 和 set-group-ID 位,假设其编译出来的可执行文件为 setids, 则执行
 * setids /etc/passwd 可以发现 /etc/passwd 并没有被设置set-user-ID 和
 * set-group-ID 位;passwd命令位于/usr/bin/目录下,执行setids /usr/bin/passwd
 * 可以发现 /usr/bin/passwd 被设置 set-user-ID 和 set-group-ID 位.其实,上面
 * 的英文说得很清楚,set-user-ID位是对可执行文件而言,不要因为 passwd 命令和
 * /etc/passwd 文件在名字上的相同而混淆了.
 */
int main(int argc, char *argv[])
{
	struct stat buf;
	char strbuf[BUFSIZ];
	char *ptr;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (stat(argv[1], &buf) < 0) {
		perror("stat error");
		exit(EXIT_FAILURE);
	}

	snprintf(strbuf, BUFSIZ, "%s: ", "是否设置了 set-user-ID");
	if (buf.st_mode & S_ISUID)
		ptr = "TRUE\n";
	else
		ptr = "FALSE\n";
	strncat(strbuf, ptr, BUFSIZ-strlen(strbuf));
	printf("%s", strbuf);

	snprintf(strbuf, BUFSIZ, "%s: ", "是否设置了 set-group-ID");
	if (buf.st_mode & S_ISGID)
		ptr = "TRUE\n";
	else
		ptr = "FALSE\n";
	strncat(strbuf, ptr, BUFSIZ-strlen(strbuf));
	printf("%s", strbuf);

	exit(EXIT_SUCCESS);
}
