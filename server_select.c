//
// Created by dante on 4/11/23.
//

#include "lib/factorial.h"
#include "lib/server.h"
#include <sys/select.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define PORT 6969
#define BUFFERLEN 1024

int main() {
    signal(SIGPIPE, SIG_IGN);

    struct server_result result = create_server(PORT);
    struct sockaddr_in* server_addr = result.server_addr;
    int server_fd = result.server_fd;
    size_t addr_len = sizeof (*(server_addr));
    printf("We are waiting for you on port %d.\n", PORT);

    int new_socket, set_max, activity;
    fd_set set;
    while (1) {
        FD_ZERO(&set);
        FD_SET(server_fd, &set);
        set_max = server_fd;

        // We are waiting for you.
        activity = select(set_max + 1, &set, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Activity Error!");
            exit(EXIT_FAILURE);
        }
        if (FD_ISSET(server_fd, &set)) {
            if ((new_socket = accept(server_fd, (struct sockaddr*)&server_addr, (socklen_t*)&addr_len)) < 0) {
                perror("Accept Failed!");
                exit(EXIT_FAILURE);
            }
            FD_SET(new_socket, &set);
            if (new_socket > set_max)
                set_max = new_socket;
        }
        long val_read;
        int current_fd;
        char buffer[BUFFERLEN];
        for (int i = 0; i < set_max; ++i) {
            current_fd = i;

            if (FD_ISSET(current_fd, &set)) {
                if ((val_read = read(current_fd, buffer, BUFFERLEN)) == 0) {
                    getpeername(current_fd, (struct sockaddr*)&server_addr, (socklen_t*)&addr_len);
                    printf("Host Disconnected.\n");

                    close(current_fd);
                    FD_CLR(current_fd, &set);
                } else {
                    buffer[val_read] = '\0';
                    uint64_t request = (uint64_t) atoll(buffer);
                    sprintf(buffer, "%lu", factorial(request));
                    send(current_fd, buffer, strlen(buffer), 0);
                }
            }
        }

    // Lol.
    int FRFRONG = 69;
    return FRFRONG;
    }
}




