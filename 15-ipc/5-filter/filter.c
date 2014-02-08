#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

/* One thing that popen() is especially well suited for is executing simple
 * filters to transform the input or output of the running command. Such is
 * the case when a command wants to build its own pipeline.
 */
int main(void)
{
    char line[1024];
    FILE *fpin;

    /* 刚开始写的时候,下面popen()的第一个参数写的是 "myuclc", 运行时总提示
     *          sh: myuclc: not found
     * 原因在于, popen()调用 execl() 函数来执行/bin/sh, 而/bin/sh 可能是调
     * 用execlp()来执行所给命令 (见书中1.8小节), execlp()的第一个参数是一个
     * 文件名,当这个文件名中不包含斜线 (/) 时, execlp()函数会在这个文件名的
     * 前面逐个添加$PATH中的路径前缀来得到可能的文件路径,并按照这个路径去查
     * 找文件.下面的 myuclc 文件是放在当前目录下,但是在我的终端上,当前目录并
     * 不包含在$PATH中,所以会找不到这个文件. 而写成 "./myuclc" 的形式后,这
     * 就已经是一个路径名了, execlp()函数不会再在它的前面添加路径前缀,而是
     * 就按照这个路径来查找这个文件,也就是在当前目录下查找,刚好能找到.
     */
    // if ((fpin = popen("myuclc", "r")) == NULL) {
    if ((fpin = popen("./myuclc", "r")) == NULL) {
        printf("popen myuclc error: %m\n");
        exit(1);
    }

    for ( ; ; ) {
        fputs("prompt> ", stdout);
        /* We need to call fflush() after writing the prompt, because the
         * standard output is normally line buffered, and the prompt does
         * ot contain a newline. 刚开始写的时候, fflush()语句是写在循环的
         * 最下面,运行时, "prompt> "字符串不会立刻打印出来,需要输入回车后
         * 再打印,和预期情况不符.
         */
        fflush(stdout);
        if (fgets(line, 1024, fpin) == NULL)
            break;
        if (fputs(line, stdout) == EOF) {
            printf("fputs error: %m\n");
            exit(1);
        }
    }

    if (pclose(fpin) < 0) {
        printf("pclose error: %m\n");
        exit(1);
    }
    putchar('\n');
    exit(0);
}
