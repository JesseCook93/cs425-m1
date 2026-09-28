/*
 * lookup.c - resolve a hostname with getaddrinfo and print every address.
 *
 * Usage: ./lookup <hostname>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <hostname>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *host = argv[1];
    struct addrinfo hints;
    struct addrinfo *res, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      /* both IPv4 and IPv6 */
    hints.ai_socktype = SOCK_STREAM;  /* one entry per address, not per socket type */

    int rc = getaddrinfo(host, NULL, &hints, &res);
    if (rc != 0) {
        fprintf(stderr, "%s: lookup of '%s' failed: %s\n",
                argv[0], host, gai_strerror(rc));
        return EXIT_FAILURE;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        char buf[INET6_ADDRSTRLEN];
        const void *addr;
        const char *label;

        if (p->ai_family == AF_INET) {
            addr = &((struct sockaddr_in *)p->ai_addr)->sin_addr;
            label = "IPv4";
        } else if (p->ai_family == AF_INET6) {
            addr = &((struct sockaddr_in6 *)p->ai_addr)->sin6_addr;
            label = "IPv6";
        } else {
            continue;
        }

        if (inet_ntop(p->ai_family, addr, buf, sizeof(buf)) == NULL) {
            perror("inet_ntop");
            continue;
        }
        printf("%s %s\n", label, buf);
    }

    freeaddrinfo(res);
    return EXIT_SUCCESS;
}
