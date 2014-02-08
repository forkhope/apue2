#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* Filter to convert uppercase characters to lowercase */
int main(void)
{
    int c;

    while ((c = getchar()) != EOF) {
        if (isupper(c))
            c = tolower(c);
        if (putchar(c) == EOF) {
            printf("putchar error: %m\n");
            exit(1);
        }
        if (c == '\n')
            fflush(stdout);
    }
    exit(0);
}
