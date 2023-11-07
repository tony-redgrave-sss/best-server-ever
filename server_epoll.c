//
// Created by dante on 4/11/23.
//

#include "lib/server.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "lib/factorial.h"

#define PORT 6969
#define MAX_EVENTS 4000
#define BUFFERLEN 1024

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
                struct epoll_event new_event;
                new_event.data.fd = events[i].data.fd;
                new_event.events = EPOLLIN;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, events[i].data.fd, &new_event) == -1) {
                    perror("epoll_ctl failed!");
                    exit(EXIT_FAILURE);
                }
            } else {
                char buffer[BUFFERLEN];
                memset(buffer, '\0', sizeof(buffer));
                int done = 0;
                ssize_t count = read(events[i].data.fd, buffer, sizeof(buffer));
                if (count == -1) {
                    done = 1;
                    break;
                } else if (count == 0) {
                    done = 1;
                    break;
                }

                int request = atoi(buffer);
                sprintf(buffer, "%lu", factorial(request));
                count = write(events[i].data.fd, buffer, strlen(buffer));
                if (count == -1) {
                    perror("Write Failed!");
                    exit(EXIT_FAILURE);
                }

                if (done) {
                    printf("Closing descriptor %d\n", events[i].data.fd);
                    close(events[i].data.fd);
                }
            }
        }
    }
    close(server_fd);
    return EXIT_SUCCESS;
}