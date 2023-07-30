/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 
/* NOT MY CODE */ 

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <poll.h>
#include "Client.hpp"

int main()
{
	int server_fd, client_socket, k;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	bind(server_fd, (struct sockaddr *)&address, sizeof(address));
	listen(server_fd, 3);
	client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
	Client client = Client(client_socket);
    while (true) {
		k = read(client_socket, buffer, 1024);
		if (!k || !client._keepAlive)
			break ;
		buffer[k - 2] = 0;
		client.execute(buffer);
    }
	close(client_socket);
	shutdown(server_fd, SHUT_RDWR);
	return 0;
}

// PASS <password>	
// 001 JohnDoe :Welcome to ExampleIRC, JohnDoe!
/*
421 JohnDoe XYZ :Unknown command
	JohnDoe nickname
	XYZ command
*/
// NICK <nickname>
// USER <username> 0 * <realname>
// nc lithium.libera.chat 6667
// nickname should be unique