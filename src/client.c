/* -*- mode: c; coding: utf-8 -*- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

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

static int connect_stream(struct addrinfo *ai) {
    int s;
    struct addrinfo *a;

    for (a = ai; a; a = a->ai_next) {
        s = socket(a->ai_family, a->ai_socktype, a->ai_protocol);
        if (s == -1)
            continue;
        if (connect(s, a->ai_addr, a->ai_addrlen) != -1)
            break; // SUCCESS
        close(s);
    }
    if (a == NULL) {
        return -1;
    }
    return s;
}

static void do_task(int s) {
    int n;
    char buf[4096];

    while ((n = read(s, buf, sizeof(buf)) - 1) > 0) {
        buf[n] = '\0';
        puts(buf);
    }
}

static void client(const char *nodename, const char *servname) {
    int s;
    struct addrinfo *ai;

    ai = getaddrinfo_stream(nodename, servname);
    s = connect_stream(ai);
    if (s == -1) {
        fprintf(stderr, "cannot connect %s\n", nodename);
        exit(1);
    }
    do_task(s);
    close(s);
    freeaddrinfo(ai);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <nodename> <servname>\n", argv[0]);
        exit(1);
    }

    client(argv[1], argv[2]);

    exit(0);
}
