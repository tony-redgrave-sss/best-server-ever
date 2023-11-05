//
// Created by dante on 4/11/23.
//

#include "lib/server.h"
#include "lib/factorial.h"
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 6969
#define BUFFERLEN 1024

// Just a global variable to see the count of people joining in.
int count = 0;

void* serve_request(void* args) {
    printf("Hello There. You are visitor number %d\n", (count++) + 1);
    int new_socket = *((int*)args);
    char buffer[BUFFERLEN] = {0};
    uint64_t request;

    read(new_socket, buffer, BUFFERLEN);
    request = (uint64_t) atoll(buffer);

    printf("Received %s. ", buffer);
    sprintf(buffer, "%lu", factorial(request));
    printf("Sending %s.\n", buffer);
    send(new_socket, buffer, strlen(buffer), 0);
    close(new_socket);

    return NULL;
}

int main() {
    // bunch of code to get details from server init
    struct server_result result = create_server(PORT);
    struct sockaddr_in* server_addr = result.server_addr;
    int server_fd = result.server_fd;
    size_t addr_len = sizeof (*(server_addr));

    printf("We are waiting for you on port %d.\n", PORT);
    int new_socket;
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&server_addr, (socklen_t*)&addr_len)) < 0) {
            perror("Accept Failed!");
            break;
        }
        pthread_t thread;
        if (pthread_create(&thread, NULL, serve_request, (void*)&new_socket) < 0) {
            perror("Thread Creation Failed!");
            break;
        }
        // The thread embarks on its journey.
    }

    // It only reached here if the server failed, but hey. There's already enough EXIT_FAILURES in the code.
    return EXIT_SUCCESS;
}
