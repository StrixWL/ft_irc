#include "Client.hpp"
#include <map>
#include <Logger.hpp>

std::vector<std::string> Client::_nickNames;

Client::Client(int &client_fd): _clientFd(client_fd), _nickName("*"), _authorized(false), _keepAlive(true) {
	this->logger = CLogger(_clientFd);
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
	this->logger.debug("Executing command line: [" + commandLine + "]");
	std::string command = commandLine.substr(0, commandLine.find(" "));
	commandLine.erase(0, command.length());
	logger.verbose("command: [" + command + "]");
	while (commandLine[0] == ' ')
		commandLine.erase(0, 1);
	logger.verbose("args: [" + commandLine + "]");
	try {
		if (command != "PASS" && command != "NICK" && command != "USER" && !_authorized) {
			throw std::logic_error("464 " + _nickName + " :You have not registered\r\n");
		}
		if (_commands[command])
			(this->*_commands[command])(commandLine);
		else {
			throw std::logic_error("421 " + _nickName + " " + command + " :Unknown command\r\n");
		}
	} catch (std::exception &e) {
			send(e.what());
			if (!_keepAlive) {
				close(_clientFd);
			}
	}
}
