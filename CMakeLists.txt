cmake_minimum_required(VERSION 3.22)
project(best-server-ever)

set(LIBS lib/factorial.c lib/server.c)

add_executable(Server_pth server_pthread.c ${LIBS})
add_executable(Server_fork server_fork.c ${LIBS})
add_executable(Server_select server_select.c ${LIBS})
add_executable(Server_poll server_poll.c ${LIBS})
add_executable(Server_epoll server_epoll.c ${LIBS})

add_executable(yeet main.c
        main.c)