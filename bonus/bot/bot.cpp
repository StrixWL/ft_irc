#include "bot.hpp"
#include <Logger.hpp>

void bot::start()
{

    if ((this->socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		logger.error("Couldn't create socket");
		exit(1);
	}


    struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, HOST, &serv_addr.sin_addr) <= 0) {
		logger.error("Invalid address");
		exit(1);
	}
    int status;
	if ((status = connect(this->socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
		logger.error("Connection Failed");
		exit(1);
	}
    if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1) {
        logger.error("Error setting socket to nonblocking mode!");
        exit(1);
    }
    logger.warn("bot has connected to the server");

    char buffer[100];
    int i = 0;
   
    std::string nick_s = "NICK ";
    std::string log_n = nick_s + NICK + "\n";
    send(this->socket_fd, log_n.c_str(), sizeof(log_n) ,0);
    recv(this->socket_fd, buffer, 100, 0);
    if (strstr(buffer, "Nickname is already in use"))
    {
        logger.error("Nickname is already in use");
        exit(1);
    }
    std::string user_s = "USER ";
    std::string log_u = user_s + u_name + " s " + "s " + r_name + "\n";
    usleep(50);
    send(this->socket_fd, log_u.c_str(), sizeof(log_u) ,0);

    std::string pass_s = "PASS ";
    std::string log_p = pass_s + PASS + "\n";
    usleep(50);
    send(this->socket_fd, log_p.c_str(), sizeof(log_p) ,0);

    bzero(buffer, 100);
    recv(this->socket_fd, buffer, 100, 0);
    if (strstr(buffer, "Password incorrect"))
    {
        logger.error("Password incorrect");
        exit(1);
    }
    logger.warn("Registered  succesfully");
    while (1)
    {

    }
}

int main()
{
 

    bot my_bot = bot();
    my_bot.start();
}