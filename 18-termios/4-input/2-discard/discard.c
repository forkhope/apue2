#include <stdio.h>
#include <unistd.h>
#include <sys/termios.h>

/* 讨论终端I/O的特殊字符 -- DISCARD
 * The discard character. This character, recognized on input in extended
 * mode (IEXTEN), causes subsequent to be discarded until another DISCARD
 * character is entered or the discard condition is cleared (see the
 * FLUSHO option). This character is discarded when processed (i.e., it is
 * not passed to the process).
 *
 * DISCARD (非标准): discard output, VDISCARD, c_lflag, IEXTEN, ^O
 * man tcgetattr 手册中提到: VDISCARD (not in POSIX; not supported under
 * Linux; 017, SI, Ctrl-O). 即该字符的值是八进制的017,十进制是15.
 * 实际测试发现,由于man手册提到,Linux不支持该字符,但是VDISCARD这个宏还是
 * 有的,我们可以使用这个宏来打印c_cc[VDISCARD]字符的值,这里说的不支持,应该
 * 是说不支持这个字符对应的功能.
 */
int main(void)
{
    struct termios term;
    long disable;

    if (isatty(STDIN_FILENO) == 0) {
        perror("the STDIN_FILENO isn't a tty");
        return 1;
    }

    if ((disable = fpathconf(STDIN_FILENO, _PC_VDISABLE)) < 0) {
        perror("fpathconf error");
        return 1;
    }

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr error");
        return 1;
    }

    /* 根据下面网址可知, Control-O 对应的值是 <SI>.
     * pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap10.html
     * 而查看 man ascii, 可知 <SI> 对应的ascii码值是 15. 下面打印出来的
     * c_cc[VDISCARD] 的值也确实是 15.即输入Control-O 就能输入DISCARD字符.
     *
     * 在bash的命令行中输入一串字符串,如ls,然后敲Control-O,则会执行该命令,
     * 如果输入的字符串不是有效命令,会提示"bash: xxx: 未找到命令".
     * 从这个现象来看,DISCARD并不是"丢弃之前所输入字符"的意思.而从上面的
     * 描述看,也可以发现DISCARD是丢弃随后输出的意思,但实际测试没有看到这个
     * 现象,目前还不清楚丢弃随后输出是怎么样的情况.
     *
     * 2013-07-29 更新
     * 根据man手册,Linux中不支持VDISCARD这个字符对应的功能.
     */
    printf("term.c_cc[VDISCARD] = %d\n", term.c_cc[VDISCARD]);
    printf("term.c_cc[VDISCARD] = %#o\n", term.c_cc[VDISCARD]);

    printf("term.c_lflag = %#x, IEXTEN = %#x\n", term.c_lflag, IEXTEN);
    printf("term.c_lflag & IEXTEN = %s\n", term.c_lflag & IEXTEN ?
            "TRUE" : "FALSE");

    return 0;
}
