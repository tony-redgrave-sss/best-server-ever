//
// Created by dante on 4/11/23.
//

#include "lib/server.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>

#define PORT 6969
#define MAX_EVENTS 4000

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
    // bunch of code to get details from server init
    struct server_result result = create_server(PORT);
    struct sockaddr_in* server_addr = result.server_addr;
    int server_fd = result.server_fd;
    size_t addr_len = sizeof (*(server_addr));

    printf("We are waiting for you on port %d.\n", PORT);

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("Epoll Failed!");
        exit(EXIT_FAILURE);
    }
    
    struct epoll_event event;
    event.data.fd = server_fd;
    event.events = EPOLLIN;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("epoll_ctl failed!");
        exit(EXIT_FAILURE);
    }

    while (1) {
        struct epoll_event events[MAX_EVENTS];
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; ++i) {
            if ((events[i].events & EPOLLERR) ||
            (events[i].events & EPOLLHUP) ||
            (events[i].events & EPOLLIN)) {
                fprintf(stderr, "epoll failed on %d\n", events[i].data.fd);
                close(events[i].data.fd);
                continue;
            } else if (server_fd == events[i].data.fd) {
                // accept conn, add to epoll
            } else {

            }

        }
    }
}