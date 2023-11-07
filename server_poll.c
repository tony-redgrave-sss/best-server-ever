//
// Created by dante on 4/11/23.
//

#include "lib/server.h"
#include "lib/factorial.h"
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define PORT 6969
#define MAX_FDS 4000
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

    struct pollfd fds[MAX_FDS];
    memset(fds, 0, sizeof(fds));
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    int n_fds = 1;
    while (1) {
        int n = poll(fds, MAX_FDS, -1);
        // inner loop
        for (int i = 0; i < n; ++i) {
            struct pollfd current = fds[i];
            if (current.revents == POLLIN) {
                if (server_fd == current.fd) {
                    // new connection! Accept and add socket to fds array
                    struct sockaddr_storage client_addr;
                    socklen_t address_len = sizeof(client_addr);
                    int new_conn_fd = accept(server_fd, (struct sockaddr*)&client_addr, &address_len);
                    if (new_conn_fd == -1) {
                        perror("Accept Failed!");
                    } else {
                        if (non_block(new_conn_fd) == -1) {
                            perror("Socket Non-Blocking Failed!");
                            close(new_conn_fd);
                        } else {
                            fds[n_fds].fd = new_conn_fd;
                            fds[n_fds++].events = POLLIN;
                        }
                    }
                } else {
                    // data waiting to be read! process and send
                    if (current.revents == POLLIN) {
                        char buffer[BUFFER_LEN];
                        memset(buffer, '\0', sizeof(buffer));
                        int done = 0;
                        ssize_t count = read(current.fd, buffer, sizeof(buffer));
                        if (count == -1) {
                            if (errno != EAGAIN) {
                                perror("Read Error!");
                                done = 1;
                            }
                            break;
                        } else if (count == 0) {
                            done = 1;
                            break;
                        }
                        int request = atoi(buffer);
                        sprintf(buffer, "%lu", factorial(request));
                        count = write(current.fd, buffer, strlen(buffer));
                        if (count == -1) {
                            perror("Write Error!");
                            exit(EXIT_FAILURE);
                        }
                        if (done) {
                            printf("Connection closed lol %d\n", current.fd);
                            close(current.fd);
                        }
                    }
                }
            }
        }
    }
}