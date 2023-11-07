//
// Created by dante on 4/11/23.
//

#include "lib/factorial.h"
#include "lib/server.h"
#include <sys/select.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#define PORT 6969
#define MAX_BUFFER 1024

int count = 0;

int main() {
    signal(SIGPIPE, SIG_IGN);

    struct server_result result = create_server(PORT);
    struct sockaddr_in server_addr = *(result.server_addr);
    int server_fd = result.server_fd;
    size_t addr_len = sizeof (server_addr);
    printf("We are waiting for you on port %d.\n", PORT);

    int new_socket, set_max, activity, i, request, bytes_read, client_socket;
    fd_set set;
    char buffer[MAX_BUFFER];
    while (1) {
        FD_ZERO(&set);
        FD_SET(server_fd, &set);
        set_max = server_fd;

        activity = select(set_max + 1, &set, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            fprintf(stderr, "Select Failed!\n");
        }
        // hello there
        if (FD_ISSET(server_fd, &set)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&server_addr, (socklen_t*)&addr_len)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
//            printf("Hello there, %s:%d using socket fd %d.\n",
//                   inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port), new_socket);
            // add to fd_set
            FD_SET(new_socket, &set);
            if (new_socket > set_max) {
                set_max = new_socket;
            }
        }

        // bruh
        for (i = 0; i <= set_max; i++) {
            client_socket = i;

            if (FD_ISSET(client_socket, &set)) {
                // I read nothing, so I'm assuming you have nothing to send.
                if ((bytes_read = read(client_socket, buffer, MAX_BUFFER)) == 0) {
                    close(client_socket);
                    FD_CLR(client_socket, &set);
                } else {
                    // Never mind, here's your factorial.
                    buffer[bytes_read] = '\0';
                    request = atoll(buffer);
                    sprintf(buffer, "%lu", factorial(request));
                    send(client_socket, buffer, strlen(buffer), 0);
                }
            }
        }
    }
    // Seriously. Be responsible with your return statements!
    int FRFRONG = 69;
    return FRFRONG;
}




