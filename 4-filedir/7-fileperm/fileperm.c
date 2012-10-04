#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

/* The st_mode value also encodes the access permission bits for the file.
 * There are nine permission bits for each file:
 * S_IRUSR:	user-read,	S_IWUSR: user-write,	S_IXUSR: user-execute
 * S_IRGRP: group-read,	S_IWGRP: group-write,	S_IXGRP: group-execute
 * S_IROTH: other-read,	S_IXOTH: other-write,	S_IXOTH: other-execute
 *
 * 按我理解,这 9 个权限是小权限,其隐含有 3 个大权限.判断一个进程是否能打开
 * 某个文件,并不是直接检查这 9 个小权限,而是先检查其隐含的 3 个大权限: 用户
 * 权限,组权限,其他权限. 例如,如果一个进程的 effective user ID 不是0,即不是
 * 根用户,则判断这个进程是否能打开某个文件时,会先判断进程的 effective user
 * ID 是否等于文件的 owner ID,如果相等,才会进一步判断 open() 函数指定的打开
 * 模式是否符合文件的 S_IRUSR, S_IWUSR, S_IXUSR 这三个小权限,符合,才能打开.
 * 关于这部分的详细描述,可参见书中第 4.5 小节.
 * 
 * 这些权限表示的是 进程 对文件的操作权限.文件本身是静态的,它自己不能操作
 * 自己,要操作一个文件,必然是在进程中操作它.例如 S_IWUSR 表示用户对文件可
 * 写,这里的"用户"就是一个进程,且必须是effective user ID等于文件 owner ID
 * 的进程. 如果进程的effective user ID不等于文件 owner ID,但是 effective
 * group ID等于文件的 owner group,则这个进程不是"用户",而是"组",对应的是
 * 文件的组权限,若文件的S_IWGRP为真,则这个进程对文件可写.
 */
int main(int argc, char *argv[])
{
	/* 书中第4.5小节提到: If the effective user ID of the process is 0
	 * (the superuser), access is allowed. This gives the superuse free
	 * rein throughout the entire file system.
	 * 当进程的 effective user ID 是 0 时,可以访问任何文件,即使这些文件没有
	 * 可执行,可读,可写权限也可以访问.下面的例子中,如果用普通用户执行,则会
	 * 打印出报错的信息,如果用root权限执行,则不会打印报错的信息,表示open()
	 * 函数执行成功,即使open()函数打开的文件是缺少相应访问权限的.
	 */

	/* Whenever we want to open any type of file by name, we must have
	 * execute permission in each directory mentioned in the name,
	 * including the current directory, if it is implied.
	 * 要打开目录里面的文件,只要对目录拥有可执行权限就可以了,不需要对
	 * 目录拥有读权限,书中描述了目录可执行权限和读权限的区别.
	 */
	if (open("noexec/x.txt", O_RDWR) < 0)
		printf("由于 noexec 目录没有可执行权限,无法打开里面的文件:%m\n");
	else
		printf("即使文件访问权限不足, root 用户也能打开文件\n");

	/* The read permission for a file determines whether we can open an
	 * existing file for reading: the O_RDONLY and O_RDWR flags for the
	 * open() function.
	 */
	if (open("a.txt", O_RDONLY) < 0)
		printf("由于 a.txt 没有读权限,使用 O_RDONLY 打开失败:%m\n");
	else
		printf("即使文件访问权限不足, root 用户也能打开文件\n");

	/* The write permission for a file determines whether we can open an
	 * existing file for writing: the O_WRONLY and O_RDWR flags for the
	 * open() function.
	 */
	if (open("a.txt", O_WRONLY) < 0)
		printf("由于 a.txt 没有写权限,使用 O_WRONLY 打开失败:%m\n");
	else
		printf("即使文件访问权限不足, root 用户也能打开文件\n");

	/***************************************/
	if (open("a.txt", O_RDWR) < 0)
		printf("由于 a.txt 没有读写权限,使用 O_RDWR 打开失败:%m\n");
	else
		printf("即使文件访问权限不足, root 用户也能打开文件\n");
	if (open("b.txt", O_RDWR) < 0)
		printf("由于 b.txt 只有读权限,使用 O_RDWR 打开失败:%m\n");
	else
		printf("即使文件访问权限不足, root 用户也能打开文件\n");
	if (open("c.txt", O_RDWR) < 0)
		printf("由于 c.txt 只有写权限,使用 O_RDWR 打开失败:%m\n");
	else
		printf("即使文件访问权限不足, root 用户也能打开文件\n");

	/* We must have write permission for a file to specify the O_TRUNC
	 * flag in the open() function. 程序中对此没有举例.
	 */

	/* We cannot create a new file in a directory unless we have write
	 * permission and execute permission in the directory.
	 */
	if (open("nowrite/t.txt", O_RDWR | O_CREAT, 0644) < 0)
		printf("由于 nowrite 目录没有写权限,无法在里面创建文件:%m\n");
	else
		printf("即使文件访问权限不足, root 用户也能打开文件\n");
	if (open("noexec/t.txt", O_RDWR | O_CREAT, 0644) < 0)
		printf("由于 noexec 目录没有可执行权限,无法在里面创建文件:%m\n");
	else
		printf("即使文件访问权限不足, root 用户也能打开文件\n");

	/* 这里说的需要对目录拥有执行和写权限,仅仅指文件所在的目录,不包括目录
     * 的上级目录.例如要在 dirparent/dirchild 目录中创建文件,需要对dirchild
	 * 目录拥有执行和写权限,对 dirparent 目录拥有执行权限,不需要对
	 * dirparent 拥有写权限.如下:
	 */
	if (open("nowrite2/execute-write/tian", O_RDWR|O_CREAT, 0644) < 0)
		printf("在nowrite2/execute-write中新建tian出错:%m\n");
	else
		printf("在nowrite2/execute-write中新建tian成功\n");

	/* To delete an existing file, we need write permission and execute
	 * permission in the directory containing the file. We do not need
	 * read permission or write permission for the file itself.
	 */

	/* Execute permission for a file must be on if we want to execute the
	 * file any of the six exec functions. The file also has to be a 
	 * regular file. 这两个情况,程序都没有举例.
	 */

	exit(1);
}
