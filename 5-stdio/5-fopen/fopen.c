#include <stdio.h>
#include <string.h>
#include <errno.h>

/* The following three functions open a standard I/O stream.
 * #include <stdio.h>
 * FILE *fopen(const char *restrict pathname, const char *restrict type);
 * FILE *freopen(const char *restrict pathname, const char *restrict type,
 * 					FILE *restrict fp);
 * FILE *fdopen(int filedes, const char *type);
 * 		All three return: file pointer if OK, NULL on error.
 * 关于这三个函数的说明,看书中第5.5小节.
 */
int main(void)
{
	FILE *fp;

	/* 使用 fopen() 函数的 "r" 打开一个文件,这个文件必须已经存在 */
	if ((fp = fopen("tian", "r")) == NULL)
		printf("fopen r tian error: %s\n", strerror(errno));

	/* 使用 fopen() 函数的 "r+" 打开一个文件,这个文件必须已经存在 */
	if ((fp = fopen("tian", "r+")) == NULL)
		printf("fopen r+ tian error: %s\n", strerror(errno));
	
	/* 使用 fopen() 函数的 "w" 打开一个文件,如果该文件不存在,则会新建一个
	 * 该文件.如果该文件已经存在,则 "w" 模式会将文件的大小置为 0.
	 * 如果"a"模式,则文件不存在时,也会新建一个该文件;如果文件已经存在,则
	 * "a"模式会将新写入的内容追加到文件末尾.
	 */
	if ((fp = fopen("tian", "w")) == NULL)
		printf("fopen w tian error: %s\n", strerror(errno));
	else
		printf("fopen w tian success\n");

	return 0;
}
