#include "Server.hpp"

void server::start(char *port, char *password)
{
    this->makeserver(port, password);
    pollfd serv_fd = {this->server_fd, POLLIN, 0};
    this->p_fd.push_back(serv_fd);

    while (true)
    {
        if (poll(this->p_fd.data(), this->p_fd.size(), -1) < 0)
        {
            logger.error("Error monitoring sockets!");
            exit(1);
        }
       for (int i = 0; i < this->p_fd.size(); i++)
        {
            if (this->p_fd[i].revents == 0)
                continue ;
            if (this->p_fd[i].revents == 1 && this->p_fd[i].fd == server_fd)
                handle_new_conection();
            if (this->p_fd[i].revents == 1 && this->p_fd[i].fd != server_fd)
                accept_message(i);
            if (this->p_fd[i].revents == 17)
                handle_disconnection(i);
        }
        for (int i = 0; i < this->all_clients.size(); i++)
        {
            if (!this->all_clients[i]->_keepAlive)
                handle_disconnection(i + 1);
        }
    }
}

void server::handle_new_conection()
{
    int fd;

    fd = accept(this->server_fd, NULL, NULL);
    if (fd < 0)
    {
        logger.error("Error accepting a new connection");
        exit (1);
    }
	
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
        logger.error("Error setting socket to nonblocking mode!");
        exit(1);
    }
    pollfd pd = {fd, POLLIN, 0};
    this->p_fd.push_back(pd);

    Client *my_client = new Client(fd);
    this->all_clients.push_back(my_client);
    logger.info("a new client has connected");
}

void server::accept_message(int i)
{
    char buffer[2];
    std::string msg;
    while (1)
    {
        bzero(buffer, 2);
        recv(p_fd[i].fd, buffer, 1, 0);
        msg.append(buffer);
		if (strchr(buffer, '\n'))
			break ;
    }
    while (msg[msg.length() - 1] == '\n' || msg[msg.length() - 1] == '\r') {
        msg = msg.substr(0, msg.length() - 1);
    }
    this->all_clients[i - 1]->execute(msg);
}

void server::handle_disconnection(int i)
{
    int fd;

    fd = p_fd[i].fd;
    delete this->all_clients[i - 1];
    // EDITED, SORRY, I HAD TO c:
    for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++) {\
        Channel *channel = *it;
        // deleting deleted clients from _operators
        for (std::vector<Client *>::iterator _it = channel->_operators.begin(); _it != channel->_operators.end(); _it++)
            if (*_it == all_clients[i - 1])
                channel->_operators.erase(_it--);
        for (std::vector<Client *>::iterator _it = channel->_members.begin(); _it != channel->_members.end(); _it++) {
            // delete him from channel
            if (*_it == all_clients[i - 1]) {
                channel->_members.erase(_it--);
                // delete channel if empty
                if (!channel->_members.size()) {
                    delete channel;
                    channels.erase(it--);
                }
            }
            // inform channel members that he left
            else
                (*_it)->send(":" + all_clients[i - 1]->_nickName + " QUIT :Quit: " + all_clients[i - 1]->_leaveMessage + "\r\n");
        }
    }
    //
    this->all_clients.erase(this->all_clients.begin() + i - 1);
    this->p_fd.erase(this->p_fd.begin() + i);
    close(fd);
   logger.info("Client has disconnected");
}

void server::makeserver(char *port, char *password)
{
    this->port = atoi(port);
    if (this->port <= 0)
    {
        logger.error("PORT ERROR!");
        exit(1);
    }
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        logger.error("Failed to create socket!");
        exit(1);
    }
    this->password = password;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(this->port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    int optValue = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optValue, sizeof(optValue)) == -1) {
        logger.error("Error calling setsockopt");
        exit(1);
    }
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

    if (fcntl(server_fd, F_SETFL, O_NONBLOCK) == -1) {
        logger.error("Error setting socket to nonblocking mode!");
        exit(1);
    }
}

server::~server(){
 
    for (int i = 0; i < this->all_clients.size(); i++)
    {
        delete all_clients[i];
        close(this->p_fd[i + 1].fd);
    }
    if (p_fd.size())
        close(p_fd[0].fd);
}

// loop on is_still alive
// (done) fix disconnection and messages problem 
// (done) set socket to nonblock
// (done) failed to bind to
// ask if i should change keep_alive
// (done) distructor
// (done) check nonblocking
// logg when succefuly passing something succefully and when a function fails
// remove strchr and bzero and strstr from this file and the bot file



int main(int arc, char **arv)
{
    if (arc != 3)
    {
        logger.error("the executable run as follows: ./ircserv <port> <password>");
        exit(1);
    }
    
    irc_server.start(arv[1], arv[2]);
}

server irc_server;
