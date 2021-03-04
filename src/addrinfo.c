/* -*- mode: c; coding: utf-8 -*- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

static void print_int(char *msg, int v) {
    printf("%s: %d, %#x\n", msg, v, v);
}

static void print_nameinfo(struct sockaddr *addr, socklen_t addrlen) {
    int g;
    char nodename[NI_MAXHOST];
    char servname[NI_MAXSERV];

    g = getnameinfo(addr, addrlen,
                    nodename, NI_MAXHOST,
                    servname, NI_MAXSERV,
                    NI_NUMERICHOST | NI_NUMERICSERV);
    if (g) {
        fprintf(stderr, "%s", gai_strerror(g));
        exit(1);
    }

    printf("ip address: %s\n", nodename);
    printf("port: %s\n", servname);
}

static void print_addrinfo(struct addrinfo *ai) {
    print_int("ai_flags", ai->ai_flags);
    print_int("ai_family", ai->ai_family);
    print_int("ai_socktype", ai->ai_socktype);
    print_int("ai_protocol", ai->ai_protocol);
    print_nameinfo(ai->ai_addr, ai->ai_addrlen);
}

static struct addrinfo *getaddrinfo_stream(const char *nodename, const char *servname) {
    int g;
    struct addrinfo hints;
    struct addrinfo *ai;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    g = getaddrinfo(nodename, servname, &hints, &ai);
    if (g) {
        fprintf(stderr, "%s", gai_strerror(g));
        exit(1);
    }

    return ai;
}

int list_addrinfo(char *nodename, char *servname) {
    struct addrinfo *ai0;
    struct addrinfo *ai;

    ai0 = getaddrinfo_stream(nodename, servname);

    for (ai = ai0; ai; ai = ai->ai_next) {
        print_addrinfo(ai);
    }

    return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <nodename> <servname>\n", argv[0]);
        exit(1);
    }

    list_addrinfo(argv[1], argv[2]);

    exit(0);
}
