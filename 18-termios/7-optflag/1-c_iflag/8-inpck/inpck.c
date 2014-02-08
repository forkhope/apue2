#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: INPCK, c_iflag, POSIX.1
 * When set, input parity checking is enabled. If INPCK is not set, input
 * parity checking is disabled.
 * Parity "generation and detection" and "input parity checking" are two
 * different things. The generation and detection of parity bits is
 * controlled by the PARENB flag. Setting this flag usually causes the
 * device driver for the serial interface to generate parity for outgoing
 * characters and to verify the parity of incoming characters. The flag
 * PARODD determines whether the parity should be odd or even. If an input
 * character arrives with the wrong parity, then the state of the INPCK
 * flag is checked. If this flag is set, then the IGNPAR flag is checked
 * (to see whether the input byte with the parity error should be ignored);
 * if the byte should not be ignored, then the PARMRK flag is checked to
 * see what characters should be passed to the reading process.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    /* 从执行结果看,终端默认没有设置该标志位 */
    printf("term.c_iflag: %#x, INPCK: %#x, &= %#x\n",
            term.c_iflag, INPCK, term.c_iflag & INPCK);

    return 0;
}
