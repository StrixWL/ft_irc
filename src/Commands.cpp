#include "Client.hpp"
#include <algorithm>

void Client::pass(std::string &commandLine) {
	if (!commandLine.length())
		throw std::logic_error("461 " + _nickName + " :Not enough parameters\r\n");
	if (commandLine[0] == ':')
		commandLine.erase(0, 1);
	_password = commandLine;
	std::cout << "password set to : [" << commandLine << "]" << std::endl;
	_authorized = true;
	if (commandLine != PASSWORD) {
		_authorized = false;
		throw std::logic_error("464 " + _nickName + " :Password incorrect\r\n");
	}
}

/*
The nickname must be composed of letters (a-z, A-Z), numbers (0-9), and certain special characters (see the next rule).
The special characters allowed in a nickname are: - [ ] \ ` ^ { } _ |
The nickname cannot start with a digit (0-9).
The maximum length of a nickname is typically 9 characters, but some IRC servers may allow longer nicknames.
*/
void Client::nick(std::string &commandLine) {
	if (commandLine[0] == ':')
		commandLine.erase(0, 1);
	else
		commandLine = commandLine.substr(0, commandLine.find(" "));
	if (!commandLine.length())
		throw std::logic_error("431 " + _nickName + " :No nickname given\r\n");
	for (std::string::iterator it = commandLine.begin(); it != commandLine.end(); it++) {
		if (std::string("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-[]\\`^{}_|").find(*it) == std::string::npos)
			throw std::logic_error("432 " + _nickName + " " + commandLine + " :Erroneus nickname\r\n");
	}
	if (!std::count(Client::_nickNames.begin(), Client::_nickNames.end(), commandLine)) {
		std::string oldNick = _nickName;
		_nickName = commandLine;
		Client::_nickNames.push_back(commandLine);
		std::cout << "nickname to set: [" << commandLine << "]" << std::endl;
		if (oldNick != "*") {
			Client::_nickNames.erase(std::find(Client::_nickNames.begin(), Client::_nickNames.end(), oldNick));
			send(":" + oldNick + " NICK " + _nickName + "\r\n");
		}
	}
	else
		throw std::logic_error("433 " + _nickName + " " + commandLine + " :Nickname is already in use\r\n");
}

void Client::user(std::string &commandLine) {
	std::cout << commandLine << std::endl;
}

