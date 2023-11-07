#include "lib/server.h"
#include "lib/factorial.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>
#include <signal.h>

#define MAX_EVENTS 4000
#define PORT 6969
#define BUFFER_LEN 1024

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
    // once again, server init code
    struct server_result result = create_server(PORT);
    struct sockaddr_in server_addr = *(result.server_addr);
    int server_fd = result.server_fd;
    size_t addr_len = sizeof (server_addr);
    printf("We are waiting for you on port %d.\n", PORT);
    // server init

    struct epoll_event event, events[MAX_EVENTS];
    int client_socket, fd_count, epoll_fd;
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll fd creation failed!");
        exit(EXIT_FAILURE);
    }

    // add server file descriptor to...thing
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_LEN];
    while (1) {
        fd_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (fd_count == -1) {
            perror("epoll_wait failed!");
            exit(EXIT_FAILURE);
        }

        for (int n = 0; n < fd_count; ++n) {
            // hello there
            if (events[n].data.fd == server_fd) {
                client_socket = accept(server_fd, (struct sockaddr *) &server_addr, (socklen_t *) &addr_len);
                if (client_socket == -1) {
                    perror("Accept failed!");
                    exit(EXIT_FAILURE);
                }

                // epoll magic
                non_block(client_socket);
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = client_socket;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event) == -1) {
                    perror("Adding client socket to epoll failed!");
                    exit(EXIT_FAILURE);
                }
            } else {
                // here you go, factorial
                int bytes_read = read(events[n].data.fd, buffer, BUFFER_LEN);
                if (bytes_read <= 0) {
                    close(events[n].data.fd);
                }
                uint64_t request = atoll(buffer);
                sprintf(buffer, "%lu", factorial(request));
                write(events[n].data.fd, buffer, strlen(buffer));
            }
        }
    }
}