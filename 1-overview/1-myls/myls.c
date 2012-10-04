#include "apue.h"
#include <dirent.h>

/* Prints the name of every file in a directory, and nothing else.
 * Note that the directory listing is not in alphabetical order.
 */
int main(int argc, char *argv[])
{
	DIR				*dp;	// 注意, DIR 结构体不需要添加 struct 关键字
	struct dirent 	*dirp;	// 而, struct dirent必须要有 struct 关键字

	if (argc != 2)
		err_quit("Usage: myls directory_name");

	/* opendir(), readdir() 函数在 dirent.h 头文件中声明, opendir()打开一个
	 * 目录,如果打开成功,则返回一个DIR *类型的指针,如果打开失败,则返回NULL,
	 * 并为errno变量赋值; readdir()函数以打开目录成功后返回的DIR *类型指针
	 * 为参数,去读取目录里面的目录项,读取成功,则返回一个struct dirent *类型
	 * 的指针,可以通过该指针获取目录项的信息,如果读取结束,则返回NULL.
	 */
	if ((dp = opendir(argv[1])) == NULL)
		err_sys("myls: can't open %s", argv[1]);
	while ((dirp = readdir(dp)) != NULL)
		/* readdir()函数每读取到一个目录项,就返回一个struct dirent *类型的
		 * 指针,一个目录项表示目录中存放的一个文件, d_name是 struct dirent
		 * 结构体的成员,表示文件的文件名.
		 */
		printf("%s\n", dirp->d_name);

	closedir(dp);	// 不要忘记了调用closedir()函数关闭打开的目录
	exit(0);
}
