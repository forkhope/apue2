#include <stdio.h>
#include <netdb.h>

void printprotoent(struct protoent *ent)
{
    char **p;

    printf("protoent: p_name: %s\n", ent->p_name);
    printf("protoent: p_aliases:");
    for (p = ent->p_aliases; *p != NULL; ++p)
        printf("\t%s\n", *p);
    printf("protoent: p_proto: %d\n", ent->p_proto);
    printf("~~~~~~~~~~~~~~~~~ `` ~~~~~~~~~~~~~~~~~\n");
}

/* We can map between protocol names and numbers with the following
 * functions.           #include <netdb.h>
 * void endprotoent(void);
 * struct protoent *getprotobyname(const char *name);
 * struct protoent *getprotobynumber(int proto);
 * struct protoent *getprotoent(void);
 * void setprotoent(int stayopen);
 *
 * These functions shall retrieve information about protocols.
 *
 * The setprotoent() function shall open a connection to database, and set
 * the next entry to the first entry. If the stayopen argument is non-zero,
 * the connection to the network protocol database shall not be closed after
 * each call to getprotoent() (either directly, or indirectly through one of
 * the other getproto*() functions), and the implementation may maintain an
 * open file descriptor for the database.
 *
 * The getprotobyname() function shall search the database from the
 * beginning and find the first entry for which the protocol name specified
 * by name matches the p_name number, opening and closing a connection to
 * the database as necessary.
 *
 * The getprotoent() function shall read the next entry of the database,
 * opening and closing a connection to the database necessary.
 *
 * The getprotobyname(), getprotobynumber(), and getprotoent() functions
 * shall each return a pointer a protoent structure, the members of which
 * shall contain the fields of an entry in the network protocol database.
 *
 * The endprotoent() function shall close the connection to the database,
 * releasing any open file descriptor.
 *
 * These functions need not be thread-safe.
 *
 * Upon successful completion, getprotobyname(), getprotobynumber(), and
 * getprotoent() return a pointer to a protoent structure if the requested
 * entry was found, and a null pointer if the end of the database was
 * reached or the requested entry was not found. Otherwise, a null pointer
 * is returned. No errors are defined.
 *
 * Linux中, struct protoent 结构体定义如下:
 * struct protoent {
 *      char    *p_name;    // official protocol name
 *      char   **p_aliases; // alias list, terminated by NULL
 *      int      p_proto;   // the protocol number
 * };
 */
int main(void)
{
    struct protoent *proto;
    int i;

    while ((proto = getprotoent()) != NULL)
        printprotoent(proto);
    endprotoent();

    printf("================= getprotobyname: name --> ip\n");
    if ((proto = getprotobyname("ip")) != NULL)
        printprotoent(proto);

    printf("================= getprotobynumber: number --> 6\n");
    if ((proto = getprotobynumber(6)) != NULL)
        printprotoent(proto);

    printf("================= setprotoent =============\n");
    if ((proto = getprotoent()) != NULL)
        printprotoent(proto);
    setprotoent(0);

    printf("================= after call the setprotoent function ==\n");
    for (i = 0; i < 2; ++i)
        if ((proto = getprotoent()) != NULL)
            printprotoent(proto);

    return 0;
}
