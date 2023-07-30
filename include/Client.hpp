#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <sys/socket.h>
#include <map>
#include <vector>
#include <unistd.h>
#include <iostream>

#define PASSWORD ":1"
#define PORT 6667

class Client {
	private:
		int _clientFd;
		std::map<std::string, void (Client::*)(std::string &)> _commands;
		std::string _nickName;
		std::string _userName;
		bool _authorized;
		static std::vector<std::string> _nickNames;
	public:
		bool _keepAlive;
		Client(int &clientFd);
		int getFd(void);
		void send(std::string msg);
		void execute(std::string commandLine);
		void pass(std::string &commandLine);
		void nick(std::string &commandLine);
		void user(std::string &commandLine);
};



#endif