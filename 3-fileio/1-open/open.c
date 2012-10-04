#include "apue.h"
#include <fcntl.h>

/* A file is opened or created by calling the open() function.
 * #include <fcntl.h>
 * int open(const char *pathname, int oflag, ... // mode_t mode )
 * The third argument is used only when a new file si being created.
 * 第二个参数表示文件的打开模式.它有三个基本模式:O_RDONLY,O_WRONLY,O_RDWR
 * 这三个模式必须有且只能同时有一个.另外,还有一些可选模式,通过或运算符组合
 * 如果文件打开成功,会返回一个文件描述符,打开失败,会返回-1,并为errno赋值.
 * The file descriptor returned by open() is guaranteed to be the
 * lowest-numbered unused descriptor.
 */

int main(void)
{
	char pathname[256];
	char readbuf[256];
	int fd, n;

	strncpy(pathname, "new.txt", 256);
	/* 此时, new.txt 文件不存在. 如果 open() 函数的模式没有包含 O_CREAT,则
	 * 试图打开一个不存在的文件, 函数会报错,返回-1,并为 errno 赋值.
	 */
	if ((fd = open(pathname, O_RDWR)) < 0)
		err_ret("open: can't open file(new.txt)");

	/* 使用可选模式O_CREAT及open()函数的第三个参数创建一个新文件.
	 * O_CREAT:	Create the file if it doesn't exist. This option requires
	 * a third argument to the open() function, the mode, which specifies
	 * the access permission bits of the new file. 此时,第三个参数表示所创
	 * 建文件的访问权限,下面的0644表示"用户可读写,组可读,其他可读".
	 *
	 * man手册提到,当指定O_CREAT时,第三个参数是必须,但实际测试,发现没有第三
	 * 个参数也可以,可以编译通过,运行不报错,文件也被创建,其权限为0400.
	 * 目前没有找到对此的说明.知道这点即可,写代码时,还是要指定第三个参数
	 */
	if ((fd = open(pathname, O_WRONLY | O_CREAT, 0644)) < 0)
		err_sys("open: can't create a new file");
	else 
		printf("open: create file new.txt SUCCESS\n");
	close(fd);

	/* 没有指定open()函数的三个基本模式之一,可以编译通过,但调用write()
	 * 函数会报错.奇怪的是,调用read()不会报错,如果文件有内容,可以正常读出,
	 * 如果文件没有内容,read()函数返回0,没有遇到过返回-1的情况,原因不明.
	 * 当然,写代码时,如果读文件内容,还是要用O_RDONLY或者O_RDWR来打开文件.
	 */
	if ((fd = open(pathname, O_CREAT)) < 0)
		err_sys("open: can't create a new file");
	if (write(fd, "tianxia", strlen("tianxia")) != strlen("tianxia"))
		err_ret("open: can't write to new.txt");  // 会进入该分支,报错.
	if ((n = read(fd, readbuf, 256)) < 0)
		err_ret("open: can't read from new.txt"); // 不会进到这个分支
	close(fd);

	/* 不能对使用 O_WRONLY 模式打开的文件调用 read() 函数,否则报错
	 * 不能对使用 O_RDONLY 模式打开的文件调用 write() 函数,否则报错
	 */
	if ((fd = open(pathname, O_WRONLY)) < 0)
		err_sys("open 'w': can't create a new file");
	if (read(fd, readbuf, 256) < 0)
		err_ret("open: can't read from new.txt when O_WRONLY");
	close(fd);
	if ((fd = open(pathname, O_RDONLY)) < 0)
		err_sys("open 'w': can't create a new file");
	if (write(fd, "tian", 4) < 0)
		err_ret("open: can't write to new.txt when O_RDONLY");
	close(fd);

	/* 上面提到,当文件不存在时,O_CREAT模式会新建一个文件.当文件存在时,
	 * 如果没有同时指定 O_EXCL 模式,则 O_CREAT 不会起到任何作用,也不会报错
	 * 如果文件存在时,同时指定 O_EXCL 和 O_CREAT 模式,会产生一个错误.
	 */
	if ((fd = open(pathname, O_CREAT | O_EXCL, 0644)) < 0)
		err_ret("open: can't create the new.txt");
	// 此时, b.txt 文件不存在, 下面的语句将会创建该文件
	if ((fd = open("b.txt", O_CREAT | O_EXCL, 0644)) < 0)
		err_sys("open: can't create a new file(b.txt)");
	else
		printf("open: create the b.txt SUCCESS\n");
	close(fd);

	/* 调用 open() 函数打开文件,得到一个文件描述符,赋给变量 fd,如果没有调用
	 * close(fd),就执行 fd = open(),不会报错,但是原来的文件描述符并没有关闭
	 * 这里只是改变了 fd 的值,让它执行文件描述符表的另一个表项而已,原先打开
	 * 的文件描述符依然在打开,但 fd 已不再关联它,如果之前没有保存 fd 的值,
	 * 则程序无法关闭那个打开的文件描述符,需要等到程序退出,由内核关闭它.
	 */

	return 0;
}
