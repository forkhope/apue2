#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: IXOFF, c_iflag, POSIX.1
 * If set, start-stop input control is enabled. When it notices that
 * the input queue is getting full, the terminal driver outputs a
 * STOP character. This character should be recognized by the device
 * that is sending the data and cause the device to stop. Later, when
 * the characters on the input queue have been processed, the terminal
 * driver will output a START character. This should cause the device
 * to resume sending data.
 * 从上面描述来看,开启IXOFF标志位后,如果驱动(driver)的输入队列满了,终端
 * driver会输出一个OUTPUT字符,然后终端设备(device)要能识别该设备,并停止
 * 再输入数据.等到终端driver处理了输入队列中的字符后,driver又输出一个
 * START字符,终端device才继续往driver里面写入数据.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从执行结果看,终端默认没有开启该标志位 */
    printf("term.c_iflag: %#x, IXOFF: %#x, &= %#x\n",
            term.c_iflag, IXOFF, term.c_iflag & IXOFF);

    term.c_iflag |= IXOFF;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("enable IXOFF: term.c_iflag: %#x, IXOFF: %#x, &= %#x\n",
            term.c_iflag, IXOFF, term.c_iflag & IXOFF);
    
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
