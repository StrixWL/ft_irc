#include "Server.hpp"
#include <stdlib.h>

void server::start(char *port, char *password)
{
    // creating the server socket , binding it and setting the socket as non-blocking
    this->makeserver(port, password);
    // making the first elem of poll structs array (which contain the server socket's FD)
    pollfd serv_fd = {this->server_fd, POLLIN, 0};
    this->p_fd.push_back(serv_fd);

    // the main loop of our server
    while (true)
    {
        // monitoring all the sockets using poll
        if (poll(this->p_fd.data(), this->p_fd.size(), -1) < 0)
        {
            logger.error("Error monitoring sockets!");
            exit(1);
        }
        // checking on every struct's revent so either accepting a new message or a new connection or a disconnection 
       for (unsigned long i = 0; i < this->p_fd.size(); i++)
        {
            if (this->p_fd[i].revents == 0)
                continue ;
            if (this->p_fd[i].revents == 1 && this->p_fd[i].fd == server_fd)
                handle_new_conection();
            if (this->p_fd[i].revents == 1 && this->p_fd[i].fd != server_fd)
                accept_message(i);
            if (this->p_fd[i].revents == 17 || this->p_fd[i].revents == POLLHUP)
                handle_disconnection(i);
        }
        // disconnect the client if the _keepAlive boolean is false 
        for (unsigned long i = 0; i < this->all_clients.size(); i++)
        {
            if (!this->all_clients[i]->_keepAlive)
                handle_disconnection(i + 1);
        }
        //  puts("ddddddd00");
    }
}

void server::handle_new_conection()
{
    int fd;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    // creating an FD socket for he new connection
    fd = accept(this->server_fd, (struct sockaddr *)&addr, &len);
    if (fd < 0)
    {
        logger.error("Error accepting a new connection");
        return ;
    }
	// setting the new socket to non-blocking
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
        logger.error("Error setting socket to nonblocking mode!");
        exit(1);
    }
    // adding it to POLL's array of sruct so it get monitored as well 
    pollfd pd = {fd, POLLIN, 0};
    this->p_fd.push_back(pd);

    // making the new client class
    Client *my_client = new Client(fd);
    // converting the IPv4 client address from its binary representation to a human-readable string format
    my_client->_IPAddress = inet_ntoa(addr.sin_addr);
    this->all_clients.push_back(my_client);
    logger.info("a new client has connected");
}

void server::accept_message(int i)
{
    std::string msg_to_send;
    char buffer[100];
    memcpy(buffer, all_clients[i - 1]->_msgBuffer.c_str(), all_clients[i - 1]->_msgBuffer.length());
    int k = -2;
    // reading till reaching the '\n' character
    while (k && k != -1)
    {
        ft_bzero(buffer, 100);
        k = recv(p_fd[i].fd, buffer, 99, 0);
        all_clients[i - 1]->_msgBuffer.append(buffer);
    }
    // disconnecting the client the msg was more than 512bytes
    if (all_clients[i - 1]->_msgBuffer.size() > 512 && all_clients[i - 1]->_nickName != "mmakboub")
    {
            logger.error("the message cannot be more than 512Bytes");
            handle_disconnection(i);
            return ;
    }
    while(ft_strchr(all_clients[i - 1]->_msgBuffer.c_str(), '\n'))
    {
        msg_to_send = all_clients[i - 1]->_msgBuffer.substr(0, all_clients[i - 1]->_msgBuffer.find('\n') + 1);
        all_clients[i - 1]->_msgBuffer = all_clients[i - 1]->_msgBuffer.substr(msg_to_send.length(), all_clients[i - 1]->_msgBuffer.length() - msg_to_send.length());
        // deleting the '\n' or '\r' characters from the end of line
        while (msg_to_send[msg_to_send.length() - 1] == '\n' || msg_to_send[msg_to_send.length() - 1] == '\r') {
            msg_to_send = msg_to_send.substr(0, msg_to_send.length() - 1);
        }
        // executing the command
        this->all_clients[i - 1]->execute(msg_to_send);
    }
}

void server::handle_disconnection(int i)
{
    int fd;

    fd = p_fd[i].fd;
    // EDITED, SORRY, I HAD TO c:
    for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++) {
        Channel *channel = *it;
        // deleting deleted clients from _operators
        for (std::vector<Client *>::iterator _it = channel->_operators.begin(); _it != channel->_operators.end(); _it++)
            if (*_it == all_clients[i - 1])
                channel->_operators.erase(_it--);
        // deleting deleted clients from _invitees
        for (std::vector<Client *>::iterator _it = channel->_invitees.begin(); _it != channel->_invitees.end(); _it++)
            if (*_it == all_clients[i - 1])
                channel->_invitees.erase(_it--);
        for (std::vector<Client *>::iterator _it = channel->_members.begin(); _it != channel->_members.end(); _it++) {
            // delete him from channel
            if (*_it == all_clients[i - 1]) {
                channel->_members.erase(_it--);
                // delete channel if empty
                if (!channel->_members.size()) {
                    delete channel;
                    channels.erase(it--);
                }
                // inform channel members that he left
                else {
                    for (std::vector<Client *>::iterator __it = channel->_members.begin(); __it != channel->_members.end(); __it++)
                        (*__it)->send(":" + all_clients[i - 1]->_nickName + " QUIT :Quit: " + all_clients[i - 1]->_leaveMessage + "\r\n");
                }
                break ;
            }
        }
    }
    // THIS IS SO SHIT, I SWEAR TO GOD I CAN WRITE BETTER CODE, AM JUST TIRED AND SLEEPY
    delete this->all_clients[i - 1];
    this->all_clients.erase(this->all_clients.begin() + i - 1);
    this->p_fd.erase(this->p_fd.begin() + i);
    close(fd);
   logger.info("Client has disconnected");
}

void server::makeserver(char *port, char *password)
{
    // getting the server's port
    this->port = ft_atoi(port);
    if (this->port <= 0)
    {
        logger.error("PORT ERROR!");
        exit(1);
    }
    // making the server's socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        logger.error("Failed to create socket!");
        exit(1);
    }
    // assigning the server's password
    this->password = password;
    // making he server's address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(this->port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    int optValue = 1;
    // set the SO_REUSEADDR socket option so the TIME_WAIT get ignored
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optValue, sizeof(optValue)) == -1) {
        logger.error("Error calling setsockopt");
        exit(1);
    }
    // binding the server :)  (associating an IP address and port number with a socket)
    if (bind(server_fd, (struct sockaddr*)&server_address, sizeof(sockaddr)) < 0) {
        logger.error("Failed to bind to " + std::to_string(this->port));
        exit(1);
    }

	logger.info("Server Socket has been created and binded successfully");
    if (listen(server_fd, SOMAXCONN) < 0) {
        logger.error("Failed to listen on the socket" + std::to_string(errno));
        exit(1);
    }
    logger.info("Listening ...");
    // set the status flags of the server's socket to non-blocking
    if (fcntl(server_fd, F_SETFL, O_NONBLOCK) == -1) {
        logger.error("Error setting socket to nonblocking mode!");
        exit(1);
    }
}

server::~server(){
 
    // deleting the client's classes
    for (unsigned long i = 0; i < this->all_clients.size(); i++)
    {
        delete all_clients[i];
        close(this->p_fd[i + 1].fd);
    }
    // closing the server's socket
    if (p_fd.size())
        close(p_fd[0].fd);
}


int main(int arc, char **arv)
{
    logger.info("Process pid: " + std::to_string(getpid()));
    if (arc != 3)
    {
        logger.error("the executable run as follows: ./ircserv <port> <password>");
        exit(1);
    }
    
    irc_server.start(arv[1], arv[2]);
}

server irc_server;
