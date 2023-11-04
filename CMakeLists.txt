cmake_minimum_required(VERSION 3.22)
project(best-server-ever)

add_executable(server_pth main.c)
add_executable(server_fork server_fork.c)