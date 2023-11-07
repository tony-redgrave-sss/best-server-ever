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
        // Clear the socket set
        FD_ZERO(&set);

        // Add master socket to set
        FD_SET(server_fd, &set);
        set_max = server_fd;

        // Wait for an activity on one of the sockets
        activity = select(set_max + 1, &set, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        // If something happened on the master socket, then it's an incoming connection
        if (FD_ISSET(server_fd, &set)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&server_addr, (socklen_t*)&addr_len)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd is %d, ip is : %s, port : %d\n", new_socket, inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

            // Send new connection greeting message
            if (send(new_socket, "Welcome to the server!\n", strlen("Welcome to the server!\n"), 0) != strlen("Welcome to the server!\n")) {
                perror("send");
            }

            puts("Welcome message sent successfully");

            // Add new socket to array of sockets
            FD_SET(new_socket, &set);

            // Update the maximum file descriptor
            if (new_socket > set_max) {
                set_max = new_socket;
            }
        }

        // Check for any I/O operation on some other socket
        for (i = 0; i <= set_max; i++) {
            client_socket = i;

            if (FD_ISSET(client_socket, &set)) {
                // Check if it was for closing, and also read the incoming message
                if ((bytes_read = read(client_socket, buffer, MAX_BUFFER)) == 0) {
                    // Somebody disconnected, get his details and print
                    getpeername(client_socket, (struct sockaddr*)&server_addr, (socklen_t*)&addr_len);
                    printf("Host disconnected, ip %s, port %d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

                    // Close the socket and mark as 0 in list for reuse
                    close(client_socket);
                    FD_CLR(client_socket, &set);
                } else {
                    // Echo back the message that came in
                    buffer[bytes_read] = '\0';
                    request = atoll(buffer);
                    sprintf(buffer, "%lu", factorial(request));
                    send(client_socket, buffer, strlen(buffer), 0);
                }
            }
        }
    }
    // Lol.
    int FRFRONG = 69;
    return FRFRONG;
}




