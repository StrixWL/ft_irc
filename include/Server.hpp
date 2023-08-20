#ifndef SERVER_H
#define SERVER_H

#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <map>
#include <string>
#include <vector>
#include <fcntl.h>
#include <sys/socket.h>
#include <poll.h>
#include <arpa/inet.h>
#include "Client.hpp"
#include "Channel.hpp"

class Client;
class server {
    public :
        int     server_fd;
        char    *password;
        int     port;

        std::vector<pollfd> p_fd;
        std::vector<Client *> all_clients;
        std::vector<Channel *> channels;

        struct sockaddr_in server_address;

        void start(char *, char *);
        void  makeserver(char *port, char*password);
        void handle_new_conection();
        void accept_message(int i);
        void handle_disconnection(int i);
        ~server();

        char	*ft_strchr(const char *s, int c);
        void	ft_bzero(void *s, size_t n);
        int     ft_atoi(const char *str);
        void    *ft_memcpy(void *dest, const void *src, size_t n);
};

extern server irc_server;

#endif