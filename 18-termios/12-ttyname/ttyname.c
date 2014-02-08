#include <stdio.h>
#include <unistd.h>

/* ttyname(): returns the pathname of the terminal device that is open
 * on a file descriptor.
 * #include <unistd.h>
 * char *ttyname(int fd);
 *      Returns: pointer to pathname of terminal, NULL on error.
 *
 * 书中给出了 ttyname() 函数的实现思路和具体代码,思路如下:
 * The technique is to read the /dev directory, looking for an entry with
 * the same device number and i-node number. Recal from Section 4.23 that
 * each file system has a unique device number (that st_dev field in the
 * stat structure), and each directory entry in that file system has a
 * unique i-node number (the st_ino field in the stat structure). We assume
 * in this function that when we hit a matching device number and matching
 * i-node number, we've located the desired directory entry. We could also
 * verify that the two entries have matching st_rdev fields (the major and
 * minor device numbers for the terminal device) and that the directory
 * entry is also a character special file. But since we've already verified
 * that the file descriptor argument is both a terminal device and a
 * character special file, and since a matching device number and i-node
 * number is unique on a UNIX system, there is no need for the additional
 * comparisons.
 *
 * The name of out terminal might reside in a subdirectory in /dev. Thus,
 * we might need to search the entire file system tree under /dev. We skip
 * several directories that might produce incorrect or odd-looking results:
 * /dev/., /dev/.., and /dev/fd. We also skip the aliases /dev/stdin,
 * /dev/stdout, and /dev/stderr, since they are symbolic links to files in
 * /dev/fd.
 */
int main(void)
{
    printf("ttyname of stdin: %s\n", ttyname(STDIN_FILENO));
    printf("ttyname of stdout: %s\n", ttyname(STDOUT_FILENO));
    printf("ttyname of stderr: %s\n", ttyname(STDERR_FILENO));
    return 0;
}
