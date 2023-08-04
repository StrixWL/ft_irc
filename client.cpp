/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */
/* CHATGPT GENERATED CODE FOR TESTING PURPOSE, THIS IS NOT MY CODE, TRY AGAIN SOMEWHERE ELSE */


#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

const int BUFFER_SIZE = 1024;

void receiveMessages(int clientSocket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0) {
            std::cerr << "Connection closed by server." << std::endl;
            break;
        }

        std::cout << buffer;
    }
}

void* receiveThreadWrapper(void* socketPtr) {
    int clientSocket = *reinterpret_cast<int*>(socketPtr);
    receiveMessages(clientSocket);
    return NULL;
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddress;

    // Create a socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }

    // Set up the server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(6667); // IRC default port
    inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr));

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Error connecting to the server." << std::endl;
        return 1;
    }

    // Create a thread to receive messages from the server
    pthread_t receiveThread;
    if (pthread_create(&receiveThread, NULL, receiveThreadWrapper, &clientSocket) != 0) {
        std::cerr << "Error creating receive thread." << std::endl;
        return 1;
    }

    // Send and receive messages
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        std::cin.getline(buffer, BUFFER_SIZE);

        // Send the message to the server
		std::string msg = buffer;
		msg += "\r\n";
        send(clientSocket, msg.c_str(), msg.length(), 0);
    }

    // Close the socket
    close(clientSocket);

    return 0;
}
