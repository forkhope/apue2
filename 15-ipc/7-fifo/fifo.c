#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* FIFOs are sometimes called named pipes. Pipes can be used only between
 * related processes when a common ancestor has created the pipe. With
 * FIFOs, however, unrelated processes can exchange data.
 * A FIFO is a type of file. One of the encodings of the st_mode member of
 * the stat structure indicates that a file is a FIFO. We can test for this
 * with the S_ISFIFO macro.
 * Creating a FIFO is similar to creating a file. Indeed, the pathname for
 * a FIFO exists in the file system.
 * #include <sys/stat.h>
 * int mkfifo(const char *pathname, mode_t mode);
 *      Returns: 0 if OK, -1 on error
 * The specification of the mode argument for the mkfifo() function is the
 * same as for the open() function. 如 S_ISUSR, S_IWUSR, S_IRGRP 等.
 *
 * Once we have used mkfifo() to create a FIFO, we open it using open().
 * Indeet, the normal file I/O functions (close, read, write, unlink, etc.)
 * all work with FIFOs.
 *
 * When we open() a FIFO, the nonblocking flag (O_NONBLOCK) affects what
 * happends.
 * (1) In the normal case (O_NONBLOCK not specified), an open() for read-
 * only blocks until some other process opens the FIFO for writing.
 * Similarly, an open() for write-only blocks until some other process opens
 * the FIFO for reading.
 * (2) If O_NONBLOCK is specified, an open() for read-only reads immediately
 * But an open() for write-only returns -1 with errno set to ENXIO if no
 * process has the FIFO open for reading.
 *
 * As with a pipe, if we write() to a FIFO that no process has open for
 * reading, the signal SIGPIPE is generated. When the last writer for a 
 * FIFO closes the FIFO, an end of file is generated for the reader of the
 * FIFO.
 *
 * 依我的看法,FIFO文件和普通文本文件的区别是,写入到FIFO文件的内容并没有保存
 * 在该文件中,也就是写入的数据不会永久存在于文件系统里面,用ls -l命令查看FIFO
 * 文件,可以发现其文件文件长度是0. 而写入普通文本文件的数据会永久保存在文件
 * 里面. 当然,FIFO文件会存在于文件系统中,除非它被删除,否则会一直存在.
 */
int main(void)
{
    int fifofd, n;
    char line[1024];

    /* 当名为 tempfifo 的文件(不局限于fifo文件,也可以是普通文本文件,或者
     * 其他文件)已经存在时, mkfifo() 函数会报错:File exists,即错误码EEXIST
     */
    if (mkfifo("tempfifo", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0) {
        printf("mkfifo error: %m\n");
        exit(1);
    }

    if ((fifofd = open("tempfifo", O_RDONLY)) < 0) {
        printf("open tempfifo error: %m\n");
        exit(1);
    }

    while ((n = read(fifofd, line, 1024)) > 0) {
        line[n] = '\0';         /* read() 函数不会自动添加'\0'. */
        printf("%s", line);
    }
    if (n < 0) {
        printf("read error: %m\n");
        exit(1);
    }

    close(fifofd);
    exit(0);
}
