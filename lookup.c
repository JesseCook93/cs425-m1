/*
 * lookup.c - resolve a hostname with getaddrinfo and print every address.
 *
 * Usage: ./lookup [-4 | -6] <hostname>
 *   -4  print only IPv4 addresses
 *   -6  print only IPv6 addresses
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static void usage(const char *prog)
{
    fprintf(stderr, "usage: %s [-4 | -6] <hostname>\n", prog);
}

int main(int argc, char *argv[])
{
    int family = AF_UNSPEC;           /* both IPv4 and IPv6 */
    int opt;

    while ((opt = getopt(argc, argv, "46")) != -1) {
        int want = (opt == '4') ? AF_INET : (opt == '6') ? AF_INET6 : -1;

        if (want == -1) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        if (family != AF_UNSPEC && family != want) {
            fprintf(stderr, "%s: -4 and -6 are mutually exclusive\n", argv[0]);
            return EXIT_FAILURE;
        }
        family = want;
    }

    if (argc - optind != 1) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *host = argv[optind];
    struct addrinfo hints;
    struct addrinfo *res, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = family;
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
