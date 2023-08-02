/* NOT MY CODE TAKEN FROM GEEKSFORGEEKS FOR REFERENCE PURPOSE */ 
/* NOT MY CODE TAKEN FROM GEEKSFORGEEKS FOR REFERENCE PURPOSE */ 
/* NOT MY CODE TAKEN FROM GEEKSFORGEEKS FOR REFERENCE PURPOSE */ 
/* NOT MY CODE TAKEN FROM GEEKSFORGEEKS FOR REFERENCE PURPOSE */ 
/* NOT MY CODE TAKEN FROM GEEKSFORGEEKS FOR REFERENCE PURPOSE */ 
/* NOT MY CODE TAKEN FROM GEEKSFORGEEKS FOR REFERENCE PURPOSE */ 
/* NOT MY CODE TAKEN FROM GEEKSFORGEEKS FOR REFERENCE PURPOSE */ 
/* NOT MY CODE TAKEN FROM GEEKSFORGEEKS FOR REFERENCE PURPOSE */ 
/* NOT MY CODE TAKEN FROM GEEKSFORGEEKS FOR REFERENCE PURPOSE */ 
/* NOT MY CODE TAKEN FROM GEEKSFORGEEKS FOR REFERENCE PURPOSE */ 
/* NOT MY CODE TAKEN FROM GEEKSFORGEEKS FOR REFERENCE PURPOSE */ 
/* NOT MY CODE TAKEN FROM GEEKSFORGEEKS FOR REFERENCE PURPOSE */ 
/* NOT MY CODE TAKEN FROM GEEKSFORGEEKS FOR REFERENCE PURPOSE */ 
/* NOT MY CODE TAKEN FROM GEEKSFORGEEKS FOR REFERENCE PURPOSE */ 
/* NOT MY CODE TAKEN FROM GEEKSFORGEEKS FOR REFERENCE PURPOSE */ 

// u better leave us alone taha 

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <thread>

#define PORT 6667

void delay(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

int main(int argc, char const* argv[])
{
	int status, valread, client_fd;
	struct sockaddr_in serv_addr;
	char buffer[1024];
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
		<= 0) {
		printf(
			"\nInvalid address/ Address not supported \n");
		return -1;
	}
// stexrandom
	if ((status
		= connect(client_fd, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr)))
		< 0) {
		printf("\nConnection Failed \n");
		return -1;
	}
	while (true) {
		valread = read(0, buffer, 1024);
		buffer[valread - 1] = 0;
		send(client_fd, buffer, strlen(buffer), 0);
		valread = read(client_fd, buffer, 1024);
		buffer[valread] = 0;
		printf("%s\n", buffer);
		delay(2000);
	}

	// closing the connected socket
	close(client_fd);
	return 0;
}
