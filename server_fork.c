//
// Created by dante on 4/11/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 6969
#define BUFFERLEN 1024

uint64_t* factorial() {
    uint64_t* table = (uint64_t*) malloc(sizeof(uint64_t) * 21);
    table[0] = 1;
    for (int i = 1; i < 21; ++i) {
        table[i] = i * table[i - 1];
    }
    return table;
}
int create_socket() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("Socket Creation Failed!");
        exit(EXIT_FAILURE);
    }
}
void listen_on(int sockfd, int n) {
    if ((listen(sockfd, 5)) < 0) {
        perror("Listen Failed!");
        exit(EXIT_FAILURE);
    }
}
uint64_t min(uint64_t m, uint64_t n) {
    return m > n ? n : m;
}
void sigchild_handler() {
    while (waitpid(-1, NULL, WNOHANG));
}


int main() {
    int serverfd, new_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFERLEN];
    uint64_t* factorial_table = factorial();

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
//    struct sigaction sa;
//    sa.sa_restorer = sigchild_handler;
//    sigemptyset(&sa.sa_mask);
//    sa.sa_flags = SA_RESTART;
//    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
//        perror("Signal Action Failed!");
//        exit(EXIT_FAILURE);
//    }

    while (1) {
        if ((new_socket = accept(serverfd, (struct sockaddr*)&server_addr, (socklen_t*)&addr_len)) < 0) {
            perror("Accept Failed!");
            exit(EXIT_FAILURE);
        }

        int pid = fork();
        if (pid < 0) {
            perror("Fork Failed!\n");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            close(serverfd);

            read(new_socket, buffer, BUFFERLEN);
            printf("Received: %s\n", buffer);

            uint64_t request = (uint64_t) atoll(buffer);
            uint64_t* table = factorial();
            sprintf(buffer, "%lu", table[min(20, request)]);
            send(new_socket, buffer, strlen(buffer), 0);
            close(new_socket);
            free(table);
            exit(EXIT_SUCCESS);
        } else {
            close(new_socket);
        }

        printf("\nHiTmAn bAnG\n");
    }
    return 0;
}
