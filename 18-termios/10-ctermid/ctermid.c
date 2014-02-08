#include <stdio.h>
#include <string.h>

static char ctermid_name[L_ctermid];

/* Implementation of the POSIX.1 ctermid() function.  */
char *l_ctermid(char *str)
{
    if (str == NULL)
        str = ctermid_name;
    return strcpy(str, "/dev/tty");
}

/* Historically, the name of the controlling terminal in most version of
 * the UNIX System has been /dev/tty. POSIX.1 provides a runtime function
 * that we can call to determine the name of the controlling terminal.
 * #include <stdio.h>
 * char *ctermid(char *ptr)
 *      Returns: pointer to name of controlling terminal on success,
 *               pointer to empty string on error
 * If ptr is non-null, it is assumed to point to an array of at least
 * L_ctermid bytes, and the name of the controlling terminal of the process
 * is stored in the array. The constant L_ctermid is defined in <stdio.h>.
 * If ptr is a null pointer, the function allocates room for the array
 * (usually as a static variable). Again, the name of the controlling
 * terminal of the process is stored in the array.
 *
 * In both cases, the starting address of the array is returned as the
 * value of the function. Since most UNIX systems use /dev/tty as the name
 * of the controlling terminal, this function is intended to aid
 * portability to other operating systems.
 *
 * Linux returns the string /dev/tty when we call ctermid().
 */
int main(void)
{
    char termbuff[L_ctermid];

    if (ctermid(termbuff) == NULL) {
        perror("ctermid error");
        return 1;
    }
    printf("ctermid returns: %s\n", termbuff);

    if (l_ctermid(termbuff) == NULL) {
        printf("l_ctermid return error\n");
        return 1;
    }
    printf("l_ctermid returns: %s\n", termbuff);

    return 0;
}
