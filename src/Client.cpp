#include "Client.hpp"
#include <map>

std::vector<std::string> Client::_nickNames;


Client::Client(int &client_fd): _clientFd(client_fd), _nickName("*"), _authorized(false), _keepAlive(true) {
	_commands.insert(std::make_pair("PASS", &Client::pass));
	_commands.insert(std::make_pair("NICK", &Client::nick));
	_commands.insert(std::make_pair("USER", &Client::user));
}

int Client::getFd(void) {
	return this->_clientFd;
}

void Client::send(std::string msg) {
	::send(_clientFd, msg.c_str(), msg.length(), 0);
}

void Client::execute(std::string commandLine) {
	std::string command = commandLine.substr(0, commandLine.find(" "));
	commandLine.erase(0, command.length());
	while (commandLine[0] == ' ')
		commandLine.erase(0, 1);
	try {
		if (command != "PASS" && !_authorized) {
			return ; // or just ignore??????
			_keepAlive = false;
			throw std::logic_error("464 " + _nickName + " :This server requires a password, please provide it before attempting to connect.\r\n");
		}
		if (_commands[command])
			(this->*_commands[command])(commandLine);
		else {
			throw std::logic_error("421 " + _nickName + " " + command + " :Unknown command\r\n");
		}
	} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			send(e.what());
			if (!_keepAlive) {
				close(_clientFd);
				std::cout << "T7AWA" << std::endl;
				while(1);
			}
	}
}
