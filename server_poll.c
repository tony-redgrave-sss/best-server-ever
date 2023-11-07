//
// Created by dante on 4/11/23.
//

#include "lib/server.h"
#include "lib/factorial.h"
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>

#define PORT 6969
#define MAX_CONNECTIONS 4000
#define BUFFER_LEN 1024

// I wrote this function for what?
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
    struct server_result result = create_server(PORT);
    int server_fd = result.server_fd;
    printf("We are waiting for you on port %d.\n", PORT);

    // init pollfd struct
    struct pollfd client_fds[MAX_CONNECTIONS];
    client_fds[0].fd = server_fd;
    client_fds[0].events = POLLRDNORM;
    for (int i = 1; i < MAX_CONNECTIONS; i++) {
        client_fds[i].fd = -1;
    }

    char buffer[BUFFER_LEN];
    while (1) {
        int next = poll(client_fds, MAX_CONNECTIONS, -1);
        if (client_fds[0].revents & POLLRDNORM) {
            int conn_fd = accept(server_fd, (struct sockaddr*)NULL, NULL);
            if (conn_fd < 0) {
                perror("Accept Failed!");
                continue;
            }
            int i;
            for (int i = 1; i < MAX_CONNECTIONS; i++) {
                if (client_fds[i].fd < 0) {
                    client_fds[i].fd = conn_fd;
                    break;
                }
            }
            client_fds[i].events = POLLRDNORM;
            if (--next <= 0)
                continue;
        }

        for (int i = 1; i < MAX_CONNECTIONS; i++) {
            int sockfd;
            if ((sockfd = client_fds[i].fd) < 0)
                continue;
            if (client_fds[i].revents & (POLLRDNORM | POLLERR)) {
                int n = read(sockfd, buffer, BUFFER_LEN);
                if (n < 0) {
                    close(sockfd);
                    client_fds[i].fd = -1;
                } else if (n == 0) {
                    close(sockfd);
                    client_fds[i].fd = -1;
                } else {
                    int num = atoi(buffer);
                    unsigned long fact = factorial(num);
                    sprintf(buffer, "%lu", fact);
                    write(sockfd, buffer, strlen(buffer));
                }
                if (--next <= 0) {
                    break;
                }
            }
        }
    }
}