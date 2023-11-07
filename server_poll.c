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
#define MAX_CLIENTS 4000
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
    // bunch of code to get details from server init
    struct server_result result = create_server(PORT);
    struct sockaddr_in* server_addr = result.server_addr;
    int server_fd = result.server_fd;
    size_t addr_len = sizeof (*(server_addr));
    printf("We are waiting for you on port %d.\n", PORT);

    struct pollfd client[MAX_CLIENTS];
    client[0].fd = server_fd;
    client[0].events = POLLRDNORM;
    for (int i = 1; i < MAX_CLIENTS; i++)
        client[i].fd = -1;

    char buffer[BUFFER_LEN];
    while (1) {
        int nready = poll(client, MAX_CLIENTS, -1);
        if (client[0].revents & POLLRDNORM) {
            int conn_fd = accept(server_fd, (struct sockaddr*)NULL, NULL);
            int i;
            for (i = 1; i < MAX_CLIENTS; i++) {
                if (client[i].fd < 0) {
                    client[i].fd = conn_fd;
                    break;
                }
            }
            client[i].events = POLLRDNORM;
            if (--nready <= 0)
                continue;
        }

        for (int i = 1; i < MAX_CLIENTS; i++) {
            int sockfd;
            if ((sockfd = client[i].fd) < 0)
                continue;
            if (client[i].revents & (POLLRDNORM | POLLERR)) {
                int n = read(sockfd, buffer, BUFFER_LEN);
                if (n < 0) {
                    close(sockfd);
                    client[i].fd = -1;
                } else if (n == 0) {
                    close(sockfd);
                    client[i].fd = -1;
                } else {
                    int num = atoi(buffer);
                    unsigned long fact = factorial(num);
                    sprintf(buffer, "%lu", fact);
                    write(sockfd, buffer, strlen(buffer));
                }
                if (--nready <= 0)
                    break;
            }
        }
    }
}