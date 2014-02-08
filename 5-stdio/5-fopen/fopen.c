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
 *
 * NOTE: 执行fp = fdopen(fd, "w")后, fd文件描述符并没有被自动关闭,因为Linux
 * 下,读写文件的系统调用函数需要文件描述符作为参数,所以,FILE结构体里面应该
 * 是有个表示文件描述符的整型成员,执行fdopen(),会将fd赋值给FILE结构体的对应
 * 成员. The fdopen() function shall associate a stream with a file
 * descriptor. 但是,执行 fclose(fp); 后,不但fp被关闭,fd也会被关闭,从结构上
 * 来说,fd是fp的一个成员,那么清理fp时,会先清理fd,所以fd也会被关闭.
 * 虽然执行 fp = fdopen(fd, "w")后,我们会有两个变量关联到同一个文件,但是并
 * 不需要、也不能同时调用fclose()和close()来关闭它们,调用过fclose()函数后,
 * 对应的文件描述符已经被关闭了,再调用close()将报错: Bad file descriptor.
 * 在 6-fclose/ 目录下对此也有说明.
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
	if ((fp = fopen("tian", "w")) == NULL) {
		printf("fopen w tian error: %s\n", strerror(errno));
        fclose(fp);
    }
	else
		printf("fopen w tian success\n");

    /* 虽然说 fopen(path, "w") 会新建一个不存在的文件,但如果path指定的
     * 路径中,包含一个不存在的目录名时,该文件会创建失败,错误码是ENOENT,
     * strerror(errno)返回No such file or directory.查看man 2 open手册,
     * 对该错误码描述为: ENOENT: O_CREAT is not set and the named file
     * does not exist. Or, a directory component in pathname does not
     * exist or is a dangling symbolic link.
     * 即, open() 和 fopen() 在新建文件时,都不会自动创建所指定路径中不
     * 存在的目录项.
     */
	if ((fp = fopen("test/tian", "w")) == NULL)
		printf("fopen test/w tian error: %s\n", strerror(errno));
	else
		printf("fopen test/w tian success\n");
	return 0;
}
