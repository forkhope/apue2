#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

/* Memory-mapped I/O lets us map a file on disk into a buffer in memory so
 * that, when we fetch bytes from the buffer, the corresponding bytes of
 * the file are read. Similarly, when we store data in the buffer, the
 * corresponding bytes are automatically written to the file. This lets us
 * perform I/O without using read() or write().
 * To use this feature, we have to tell the kernel to map a given file to a
 * region in memory. This is done by the mmap() function.
 * #include <sys/mman.h>
 * void *mmap(void *attr, size_t len, int prot, int flag, int fd, off_t off)
 *    Returns: starting address of mapped region if OK, MAP_FAILED on error
 * The addr argument lets us specify the address of where we want the mapped
 * region to start. We normally set this to 0 to allow the system to choose
 * the starting address. The return value of this function is the staring
 * address of the mapped area.
 * The fd argument is the file descriptor specifying the file that is to be
 * mapped. We have to open this file before we can map it into the address
 * space. The len argument is the number of bytes to map, and off is the
 * starting offset in the file of the bytes to map.
 * The prot argument specifies the protection of the mapped region. We can
 * specify the protection as either PROT_NONE or the bitwise OR of any
 * combination of PROT_READ, PROT_WRITE, and PROT_EXEC. The protection
 * specified for a region can't allow more access that the open() mode of
 * the file. For example, we can't specify PROT_WRITE if the file was opened
 * read-only.
 *      PROT_READ:      Region can be read.
 *      PROT_WRITE:     Region can be written.
 *      PROT_EXEC:      Region can be executed.
 *      PROT_NONE:      Region cannot be accessed.
 * The flag argument affects various attributes of the mapped region.
 * MAP_FIXED: The return value must equal addr. Use of this flag is
 * discouraged, as it hinders portability.
 * MAP_SHARED: Share this mapping. Updates to the mapping are visible to
 * other processes that map this file, and are carried through to the
 * underlying file. The file may not actually be updated until msync() [or
 * munmap()](man手册中提到了or munmap(),但是APUE强调munmap()不会引起映射区域
 * 的内容写到文件,两者描述冲突,所以对此尚有疑问) is called. Either this flag
 * or the next (MAP_PRIVATE), but not both, must be specified.
 * MAP_PRIVATE: Creates a private copy-on-write mapping. Updates to the map-
 * ping are not visible to other processes mapping the same file, and are
 * not carried through to the underlying file.
 * The value of off and the value of addr (if MAP_FIXED is specified) are
 * required to be multiple of the system's virtual memory page size. This
 * value can be obtained from the sysconf() function with an argument of
 * _SC_PAGESIZE or _SC_PAGE_SIZE. Since off and addr are often specified as
 * 0, this requirement is not a big deal.
 * WE CANN'T APPEND TO A FILE WITH mmap().
 *
 * Two signals are normally used with mapped regions. SIGSEGV is the signal
 * normally used to indicate that we have tried to access memory that is not
 * available to us. This signal can also be generated if we try to store
 * into a mapped region that we specified to mmap as read-only. The SIGBUS
 * signal can be generated if we access a portion of the mapped region that
 * does not make sense at the time of the access. For example, assume that
 * we map a file using the file's size, but before we reference the mapped
 * region, the file's size is truncated by some other process. If we then
 * try to access the memory-mapped region corresponding to the end portion
 * of the file that was truncated, we'll receive SIGBUS.
 *
 * A memory-mapped region is inherited by a child across a fork() (since
 * it's part of the parent's address space), but for the same reason, is not
 * inherited by the new program across an exec.
 *
 * We can change the permissions on an existing mapping by calling mprotect.
 * #include <sys/mman.h>
 * int mprotect(void *addr, size_t len, int prot);
 *      Returns: 0 if 0K, -1 on error
 *
 * If the pages in a shared mapping have been modified, we can call msync()
 * to flush the changes to the file that backs the mapping. The msync()
 * function is similar to fsync(), but works on memory-mapped regions.
 * int msync(void *addr, size_t len, int flags);
 *      Return: 0 if 0K, -1 on error.
 * If the mapping is private, the file mapped is not modified. As with the
 * other memory-mapped functions, the address must be aligned on a page
 * boundary.
 * The flags argument allows us some control over how the memory is flushed.
 * We can specify the MS_ASYNC flag to simply schedule the pages to be
 * written. If we want to wait for the writes to complete before returning,
 * we can use the MS_SYNC flag. Either MS_ASYNC or MS_SYNC must be specified
 *
 * A memory-mapped region is automatically unmapped when the process termi-
 * nates or by calling munmap() directly. Closing the file descriptor fd
 * does not unmap the region.
 * #include <sys/mman.h>
 * int munmap(caddr_t addr, size_t len);
 *      Returns: 0 if OK, -1 on error.
 * munmap() does not affect the object that was mpped--that is, the call to
 * munmap() does not cause the contents of the mapped region to be written
 * to the disk file. The updating of the disk file for a MAP_SHARED region
 * happens automatically by the kernel's virtual memory algorithm as we
 * store into the memory-mapped region. Modifications to memory in a
 * MAP_PRIVATE region are discarded when the region is unmapped.
 */
int main(int argc, char *argv[])
{
    int fdin, fdout;
    char *dst, *src;
    struct stat statbuf;

    if (argc != 3) {
        printf("usage: %s <fromfile> <tofile>\n", argv[0]);
        return 1;
    }

    if ((fdin = open(argv[1], O_RDONLY)) < 0) {
        printf("can't open %s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    if ((fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0) {
        printf("can't open %s: %s\n", argv[2], strerror(errno));
        return 1;
    }

    if (fstat(fdin, &statbuf) < 0) {    /* need size of input file */
        printf("can't stat %s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    /* set size of output file. 由于mmap()函数不能扩展文件大小,如果mmap()
     * 映射的区域大于文件大小,则会超出文件之外的范围读写时,这些操作将不会
     * 反应到文件内容上,也就是这些操作对文件没有影响.
     */
    if (lseek(fdout, statbuf.st_size - 1, SEEK_SET) == -1) {
        printf("can't lseek %s: %s\n", argv[2], strerror(errno));
        return 1;
    }
    if (write(fdout, "", 1) != 1) {
        printf("can't write %s: %s\n", argv[2], strerror(errno));
        return 1;
    }

    if ((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, 
                    fdin, 0)) == MAP_FAILED) {
        printf("can't mmap %s: %s\n", argv[2], strerror(errno));
        return 1;
    }

    /* man mmap 中提到, 如果指定了 MAP_SHARED 和 PROT_WRITE 时,文件描述符
     * 就应该是 O_RDWR 模式,否则mmap()将会报错,错误码为 EACCES.
     */
    if ((dst = mmap(0, statbuf.st_size, PROT_WRITE,
                    MAP_SHARED, fdout, 0)) == MAP_FAILED) {
        printf("can't mmap %s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    memcpy(dst, src, statbuf.st_size);  /* does the file copy */

    /* 下面一句会在运行时导致 "段错误",因为 src 对应的映射区域是只读的,所以
     * 执行写操作会引起 SIGSEGV 信号,该信号默认终止整个进程.
     */
    // src[0] = 'A';

    /* 下面一条语句也会报错: Permission denied, 即错误码EACCES. 因为src
     * 对应的文件描述符是用只读模式打开的,不能对它应用PROT_WRITE属性.
     */
    if (mprotect(src, statbuf.st_size, PROT_WRITE) < 0) {
        printf("src: mprotect error: %s\n", strerror(errno));
    }

    /* 很奇怪, dst 上面是用 PROT_WRITE 来映射的,但是下面是用 dst[1] 来读
     * 也是可读的. 原因未明.
     */
    printf("dst[1]: %c\n", dst[1]);

    if (munmap(dst, statbuf.st_size) < 0) {
        printf("dst: munmap error: %s\n", strerror(errno));
        return 1;
    }
    close(fdout);

    /* 由于后面的mmap()语句里面同时指定了 MAP_SHARED 和 PROT_WRITE,所以
     * 下面的open()一定要指定 O_RDWR 模式,否则运行报错: Permission denied
     */
    // if ((fdout = open(argv[2], O_WRONLY)) < 0) {
    if ((fdout = open(argv[2], O_RDWR)) < 0) {
        printf("dst: can't open %s: %s\n", argv[2], strerror(errno));
        return 1;
    }

    /* 下面这条语句在运行时会报错:Invalid argument,即错误码为EINVAL.
     * 这是因为下面语句中的off参数的值14,不是系统虚拟页面大小的整数倍.
     * 而这一点是强制要求的,否则mmap()函数就会报错.
     */
    // if ((dst = mmap(0, statbuf.st_size, PROT_WRITE, 
    //               MAP_SHARED, fdout, 14)) == MAP_FAILED) {
    if ((dst = mmap(0, statbuf.st_size, PROT_WRITE, 
                    MAP_SHARED, fdout, 0)) == MAP_FAILED) {
        printf("dst: mmap error: %s\n", strerror(errno));
        return 1;
    }

    return 0;
}
