#include "lib/server.h"
#include "lib/factorial.h"
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <signal.h>

#define MAX_EVENTS 10
#define PORT 6969

int non_block(int server_fd) {
    int old_flags, new_flags;

    old_flags = fcntl(server_fd, F_GETFL, 0);
    if (old_flags == -1) {
        perror("fcntl getting failed!");
        return -1;
    }

    old_flags |= O_NONBLOCK;
    new_flags = fcntl(server_fd, F_SETFL, old_flags);
    if (new_flags == -1) {
        perror("fcntl setting failed!");
        return -1;
    }
    return 0;
}

int main() {
    signal(SIGPIPE, SIG_IGN);
    // bunch of code to get details from server init
    struct server_result result = create_server(PORT);
    struct sockaddr_in server_addr = *(result.server_addr);
    int server_fd = result.server_fd;
    size_t addr_len = sizeof (server_addr);

    printf("We are waiting for you on port %d.\n", PORT);

    struct epoll_event ev, events[MAX_EVENTS];
    int conn_sock, nfds, epollfd;
    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }

    while (1) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == server_fd) {
                conn_sock = accept(server_fd, (struct sockaddr *) &server_addr, (socklen_t *) &addr_len);
                if (conn_sock == -1) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                non_block(conn_sock);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_sock;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
                    perror("epoll_ctl: conn_sock");
                    exit(EXIT_FAILURE);
                }
            } else {
                int num;
                read(events[n].data.fd, &num, sizeof(num));
                uint64_t fact = factorial(num);
                write(events[n].data.fd, &fact, sizeof(fact));
            }
        }
    }
}