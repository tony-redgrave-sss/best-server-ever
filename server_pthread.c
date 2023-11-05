//
// Created by dante on 4/11/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "lib/factorial.h"
#include "lib/server.h"

#define PORT 42069
#define BUFFERLEN 1024

int count = 0;

void* serve_request(void* args) {
    printf("Hello There. You are visitor number %d\n", (count++) + 1);
    int new_socket = *((int*)args);
    char buffer[BUFFERLEN] = {0};
    uint64_t request;

    read(new_socket, buffer, BUFFERLEN);
    request = (uint64_t) atoll(buffer);

    sprintf(buffer, "%lu", factorial(request));
    send(new_socket, buffer, strlen(buffer), 0);
    close(new_socket);

    return NULL;
}

int main() {
    signal(SIGPIPE, SIG_IGN);
    int serverfd, new_socket;
    struct sockaddr_in server_addr;

    serverfd = create_socket();
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if ((bind(serverfd, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0) {
        perror("Bind Failed!");
        exit(EXIT_FAILURE);
    }

    listen_on(serverfd, 5);
    printf("Server listening on port %d.\n", PORT);

    size_t addr_len = sizeof(server_addr);
    while (1) {
        if ((new_socket = accept(serverfd, (struct sockaddr*)&server_addr, (socklen_t*)&addr_len)) < 0) {
            perror("Accept Failed!");
        }
        pthread_t thread;
        if (pthread_create(&thread, NULL, serve_request, (void*)&new_socket) < 0) {
            perror("Thread Creation Failed!");
            exit(EXIT_FAILURE);
        }
        // yeet
    }
    return 0;
}
