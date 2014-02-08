#include <stdio.h>
#include <unistd.h>     /* fpathconf(), isatty() */
#include <termios.h>

/* This program disables the interrupt character and sets the end-of-file
 * character to Control-B.
 * Note the following in this program.
 * 1. We modify the terminal characters only if standard input is a
 * terminal device. We call isatty() to checkout this.
 * 2. We fetch the _POSIX_VDISABLE value using fpathconf.
 * 3. The function tcgetattr() fetches a termios structure from the kernel.
 * After we've modified this structure, we call tcsetattr() to set the
 * attributes. The only attributes that change are the ones we specifically
 * modified.
 * 4. Disabling the interrupt key is different from ignoring the interrupt
 * signal. This program simply disables the special character that causes
 * the terminal driver to generate SIGINT. We can still use the kill()
 * function to send the signal to the process.
 *
 * -- Disable interrupt character and change end-of-file character --
 */
int main(void)
{
    struct termios term;
    long vdisable;

    /* The isatty() function tests whether fd is an open file descriptor
     * referring to a terminal.
     * isatty() returns 1 if fd is an open file descriptor referring to a
     * terminal; otherwise 0 is returned.
     */
    if (isatty(STDIN_FILENO) == 0) {
        perror("standard input is not a terminal device");
        return 1;
    }

    /* long fpathconf(int fd, int name);
     * fpathconf() gets a value for the configuration option name for the
     * open file descriptor fd.
     * 当 name 是 _PC_VDISABLE 时, fpathconf()函数的返回值是:
     * returns nonzero if special character processing can be disabled,
     * where fd must refer to a terminal.
     */
    if ((vdisable = fpathconf(STDIN_FILENO, _PC_VDISABLE)) < 0) {
        perror("fpathconf error or _POSIX_VDISABLE not in effect");
        return 1;
    }

    if (tcgetattr(STDIN_FILENO, &term) < 0) {   /* fetch tty state */
        perror("tcgetattr error");
        return 1;
    }

    term.c_cc[VINTR] = vdisable;    /* disable INTR character */

    /* 将c_cc[VEOF]设成2,它会对应Control-B,这是因为Control-B这个字符在
     * ASCII表中的值就是2.在POSIX.1标准中有个"Control Character Names"
     * 表,里面提到<control>-B对应的Value是<STX>,查看man ascii手册,发现
     * STX字符对应的ASCII码值是2. 所以下面设置2,就对应Control-B.
     * POSIX.1的"Control Character Names"表可查看下面的网址:
     * pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap10.html
     */
    term.c_cc[VEOF] = 2;            /* EOF is Control-B */

    /* 从执行来看,这个设置不只是在进程内有效,当这个程序执行结束后,用
     * stty -a 命令查看终端窗口属性,发现属性变成了 eof = ^B; intr = <undef>
     * 然后执行cat命令时,输入Control-D,不再能够终止输入了,需要输入Contorl-B
     * 输入Control-C也不能终止程序了.
     * 从中可见,设置终端IO属性时,对整个终端产生了实际改变,进程结束后,这个
     * 改变依然有效.即, 终端IO属性并不跟进程关联在一起.
     * 由于linux的终端窗口是伪终端,这个设置只影响当前的终端窗口,其他的终端
     * 窗口属性不变.
     */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetatttr error");
        return 1;
    }

    return 0;
}
