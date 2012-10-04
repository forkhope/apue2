#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

/* 文件描述符属于进程属性,一个文件描述符的值只在当前进程内有意义.不同进程中
 * 的文件描述符值即使相等,它们也不一定就指向相同的文件.
 * 书中3.10小节提及,"The UNIX System supports the sharing of open files
 * among different processes.", 是指不同进程的文件描述符(它们的值可能相等,
 * 也可能不相等,取决于进程此时文件描述符的使用情况)可以操作同一个文件.此时
 * 每个文件描述符各自指向一个不同的file table entry,这些不同的file table
 * entry指向同一个v-node table,每个file table entry包含独有的current file
 * offset;在多个进程的不同文件描述符同时写一个文件时,这可能会出现覆盖现象.
 * 假设两个进程的两个文件描述符同时打开一个文件,且它们的current file offset
 * 相同,一个文件描述符执行完写入操作后,仅仅会增加它对应的current file offset
 * 另一个文件描述符的current file offset不变,这是,这个文件描述符的写入位置
 * 会从它的current file offset开始,将会覆盖前一个文件描述符写入的内容.
 */
int main(void)
{
	int fd, i, ui1, ui2;

	if ((fd = open("yu", O_RDWR | O_CREAT, 0644)) < 0) {
		printf("sharing a: can't open or create yu:%s\n",strerror(errno));
		return 1;
	}
	for (i = 0; i < 3; ++i) {
		/* a.c 文件写入的 4 个字符 "tian", b.c 文件写入的 2 个字符 "TI",
		 * 由于存在上面说的覆盖情况, 当执行 ./a & ./b& 时, a.c 写入的 
		 * tiantiantian, 会被 b.c 改写为 TITITIantian. 具体会怎么覆盖,
		 * 取决于这两个程序的运行先后顺序.
		 */
		if (write(fd, "tian\n", 5) != 5) {
			printf("sharing a: can't write to yu:%s\n",strerror(errno));
			return 1;
		}
		sleep(1);
	}
	close(fd);

	/* 上面描述的是两个进程的两个文件描述符操作同一个文件的情况,这同样适用
	 * 于同一个进程两个文件描述符操作同一个文件的情况.如下面的代码所示,同
	 * 一个文件打开两次,则这两个文件描述符会分别指向两个file table entry,
	 * 对其中一个文件描述符进行操作,只会增加该文件描述符对应的current file
	 * offset;分别用这两个文件描述符来执行写入操作,也会发生覆盖的现象.
	 */
	if ((ui1 = open("ui", O_RDWR | O_CREAT, 0644)) < 0) {
		printf("sharing a: can't open or create ui:%s\n",strerror(errno));
		return 1;
	}
	if ((ui2 = open("ui", O_RDWR, 0644)) < 0) {
		printf("sharing a: can't open or create ui:%s\n",strerror(errno));
		return 1;
	}
	for (i = 0; i < 3; ++i) {
		if (write(ui1, "tian\n", 5) != 5) {
			printf("sharing a: can't write to ui:%s\n",strerror(errno));
			return 1;
		}
		if (write(ui2, "UI", 2) != 2) { // 这将会之前上面写入的内容
			printf("sharing a: can't write to ui:%s\n",strerror(errno));
			return 1;
		}
	}
	close(ui1);
	close(ui2);

	printf("sharing a: finished");
	return 0;
}
