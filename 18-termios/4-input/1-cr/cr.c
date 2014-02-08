#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/termios.h>

void read_write(void)
{
    char buf[BUFSIZ];
    int n;

    while ((n = read(STDIN_FILENO, buf, BUFSIZ)) > 0)
        if (write(STDOUT_FILENO, buf, n) != n) {
            perror("write STDOUT_FILENO error");
            exit(1);
        }
    if (n < 0) {
        perror("read STDIN_FILENO error");
        exit(1);
    }
}

void whether_cr2nl(struct termios term)
{
    /* 查看终端输入是否将CR映射为NL */
    printf("term.c_iflag = %#x, ICRNL = %#x\n", term.c_iflag, ICRNL);
    printf("inupt: map CR to NL? ---- %s\n",
            (term.c_iflag & ICRNL) ? "YES" : "NO");

    /* 查看终端输出是否将CR映射为NL */
    printf("term.c_oflag = %#x, OCRNL = %#x\n", term.c_oflag, OCRNL);
    printf("output: map CR to NL? ---- %s\n",
            (term.c_oflag & OCRNL) ? "YES" : "NO"); 
}

/* 讨论终端I/O的特殊字符 -- CR
 * The carriage return character. We cannot change this character. This
 * character is recognized on input in canonical mode. When both ICANON
 * (canonical mode) and ICRNL (map CR to NL) are set and IGNCR (ignore CR)
 * is not set, the CR character is translated to NL and has the same
 * effect as a NL character. This character is returned to the reading
 * process (perhaps after being translated to a NL).
 *
 * CR: carriage return, (can't change), ICANON, \r
 * 键盘上的回车键对应的就是这个字符."carriage return"翻译过来就是"回车"
 *
 * 所谓"We cannot change this character."指的是在termios结构体的c_cc[]数组
 * 中,没有定义对应该字符的下标.例如c_cc[VEOF]会对应EOF字符,但是没有一个类似
 * VCR之类的下标来对应CR字符. 所以我们也就无法操作它.
 */
int main(void)
{
    struct termios term, oldterm;

    if (isatty(STDIN_FILENO) == 0) {
        perror("the STDIN_FILENO isn't a tty");
        return 1;
    }

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr error");
        return 1;
    }
    oldterm = term;

    /* 根据下面的网址:
     * pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap10.html
     * 可知,CR字符对应的键位是Ctrl-M.当终端将CR映射成NL时,我们可以通过
     * 输入CR字符来达到NL字符的功能,例如cat命令里面,输入一行字符串后,
     * 输入CR字符(即Ctrl-M,或者回车),这行字符串就会被回显.
     * 但是当下面将终端设置成不映射CR成NL后,执行cat命令,输入一行字符串,
     * 再输入CR字符,这行字符串将不会被回显,而是会显示"^M"这两个字符.
     * 此时,若想让cat命令接收输入,回显字符串,可以按下Ctrl-J.这是因为
     * 根据上面的网址可知,换行符(linefeed,缩写为LF)对应的键位是Ctrl-J.
     * 
     * 2013-09-05 更新
     * 而且要注意的是,当我们输入Ctrl-M,字符串没有被回显时,再输入一行
     * 字符串,然后输入Ctrl-J,会发现Ctrl-M之前的字符串没有被回显,如下:
     * tian^Mxia^Myouqingren ///  这个输入的字符串
     * youqingren            ///  这是按下Ctrl-J后回显出来的字符串
     */
    whether_cr2nl(term);

    /* 在查找到的行中,会看到 icrnl, 也就是cr映射成了nl */
    system("stty -a | grep icrnl");
    read_write();

    /* 让终端不将CR映射成NL */
    term.c_iflag &= ~ICRNL;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    whether_cr2nl(term);
    printf("------------------: clear the ICRNL flag\n");

    /* 在查找到的行中,会看到 -icrnl, 也就是cr没有映射成了nl */
    system("stty -a | grep icrnl");
    read_write();

    /* 恢复终端原来的设置 */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("restore old terminal options error");
        return 1;
    }
    return 0;
}
