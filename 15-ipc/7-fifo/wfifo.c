#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
    int fd, n;
    char line[1024];

    if ((fd = open("tempfifo", O_WRONLY)) < 0) {
        printf("open tempfifo error: %m\n");
        exit(1);
    }

    while ((n = read(STDIN_FILENO, line, 1024)) > 0) {
        if (write(fd, line, n) != n) {
            printf("write error: %m\n");
            exit(1);
        }
    }
    if (n < 0) {
        printf("read error: %m\n");
        exit(1);
    }

    exit(0);
}
