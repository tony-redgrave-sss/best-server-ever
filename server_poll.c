//
// Created by dante on 4/11/23.
//

#include "lib/factorial.h"
#include "lib/server.h"
#include <string.h>
#include <unistd.h>
#include <poll.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define PORT 6969

int main() {
    struct server_result result = create_server(PORT);
    int server_fd = result.server_fd;
    printf("We are waiting for you on port %d.\n", PORT);

    struct pollfd client[MAX_CLIENTS];
    client[0].fd = server_fd;
    client[0].events = POLLRDNORM;
    for (int i = 1; i < MAX_CLIENTS; i++) {
        client[i].fd = -1;
    }

    char buffer[BUFFER_SIZE];
    while (1) {
        int next = poll(client, MAX_CLIENTS, -1);
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
            if (--next <= 0)
                continue;
        }

        for (int i = 1; i < MAX_CLIENTS; i++) {
            int sockfd;
            if ((sockfd = client[i].fd) < 0)
                continue;
            if (client[i].revents & (POLLRDNORM | POLLERR)) {
                int bytes_read = read(sockfd, buffer, BUFFER_SIZE);
                // I read nothing, or you're lying to me about being open. I swear.
                if (bytes_read <= 0) {
                    close(sockfd);
                    client[i].fd = -1;
                } else {
                    // No? Here's your factorial.
                    uint64_t request = atoll(buffer);
                    sprintf(buffer, "%lu", factorial(request));
                    write(sockfd, buffer, strlen(buffer));
                }
                if (--next <= 0) {
                    break;
                }
            }
        }
    }
}
