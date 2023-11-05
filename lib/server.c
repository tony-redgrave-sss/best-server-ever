//
// Created by dante on 4/11/23.
//

#include "server.h"
#include <string.h>

int create_socket() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("Socket Creation Failed!");
        exit(EXIT_FAILURE);
    }
    return fd;
}

int listen_on(int sockfd, int n) {
    if ((listen(sockfd, 5)) < 0) {
        perror("Listen Failed!");
        exit(EXIT_FAILURE);
    }
    return 1;
}

struct sockaddr_in* create_sockaddr(uint16_t port) {
    struct sockaddr_in *server_addr = (struct sockaddr_in*) malloc(sizeof (struct sockaddr_in));
    memset(server_addr, 0, sizeof(struct sockaddr_in));
    server_addr->sin_port = htons(port);
    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = htonl(INADDR_ANY);

    return server_addr;
}

void bind_socket(int server_fd, struct sockaddr* server_addr) {
    if (bind(server_fd, server_addr, sizeof(*(server_addr))) < 0) {
        perror("Bind Failed!");
        exit(EXIT_FAILURE);
    }
}

void socket_option(int sockfd, char* opt) {
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed!\n");
        exit(EXIT_FAILURE);
    }
}

/**Returns the file descriptor for the server.
 */
struct server_result create_server(uint16_t port) {
    char opt = 1;
    int serverfd = create_socket();
    struct sockaddr_in* server_addr = create_sockaddr(port);
    socket_option(serverfd, &opt);
    bind_socket(serverfd, (struct sockaddr*)server_addr);
    listen_on(serverfd, 5);

    struct server_result result;
    result.server_fd = serverfd;
    result.server_addr = server_addr;

    return result;
}






