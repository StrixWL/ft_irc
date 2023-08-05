#ifndef BOT_HPP
#define BOT_HPP

#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
 #include <fcntl.h>
 #include <chrono>
#include <thread>
#include <cstdlib>
#define HOST "127.0.0.1"
#define PORT 8181
#define NICK "yasrix"
#define r_name "yasrix"
#define u_name "yasrix"
#define PASS "123"

class bot{
    public :
    int socket_fd;
    void    start();
};

#endif