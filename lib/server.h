//
// Created by dante on 4/11/23.
//

#ifndef BEST_SERVER_EVER_SERVER_H
#define BEST_SERVER_EVER_SERVER_H

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int create_socket();
int listen_on(int sockfd, int n);
struct sockaddr_in* create_sockaddr(uint16_t port);
void bind_socket(int server_fd, struct sockaddr* server_addr);
int create_server(uint16_t port);


#endif //BEST_SERVER_EVER_SERVER_H
