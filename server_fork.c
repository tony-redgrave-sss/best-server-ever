//
// Created by dante on 4/11/23.
//

#include "lib/factorial.h"
#include "lib/server.h"
#include <unistd.h>
#include <string.h>

#define PORT 6969
#define BUFFERLEN 1024

int main() {
    struct server_result result = create_server(PORT);
    struct sockaddr_in* server_addr = result.server_addr;
    int server_fd = result.server_fd;
    size_t addr_len = sizeof (*(server_addr));

    printf("We are waiting for you on port %d.\n", PORT);
    int new_socket;
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&server_addr, (socklen_t*)&addr_len)) < 0) {
            perror("Accept Failed!");
            exit(EXIT_FAILURE);
        }

        int pid = fork();
        if (pid < 0) {
            perror("Fork Failed!");
            break;
        }
        if (pid == 0) {
            close(server_fd);

            char buffer[BUFFERLEN];
            read(new_socket, buffer, BUFFERLEN);
            printf("Received %s...", buffer);
            uint64_t request = (uint64_t) atoll(buffer);
            sprintf(buffer, "%lu", factorial(request));
            printf("Sending %s.\n", buffer);
            send(new_socket, buffer, strlen(buffer), 0);
            close(new_socket);

            exit(EXIT_SUCCESS);
        } else {
            close(new_socket);
        }
    }

    return EXIT_SUCCESS;
}
