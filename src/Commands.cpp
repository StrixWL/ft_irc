#include "Client.hpp"

void Client::pass(std::string &commandLine) {
	if (_authorized)
		throw std::logic_error("462 " + _nickName + " :You may not reregister\r\n");
	if (!commandLine.length())
		throw std::logic_error("461 " + _nickName + " :Not enough parameters\r\n");
	if (commandLine[0] == ':')
		commandLine.erase(0, 1);
	if (commandLine != PASSWORD) {
		std::cout << commandLine << ", " << PASSWORD << std::endl;
		throw std::logic_error("464 " + _nickName + " :Password incorrect\r\n");
	}
	else
		_authorized = true;
}

void Client::nick(std::string &commandLine) {
	Client::_nickNames.push_back("taken");
	if (!commandLine.length())
		throw std::logic_error("431 " + _nickName + " :No nickname given\r\n");
	if (commandLine[0] == ':')
		commandLine.erase(0, 1);
	if (!std::count(Client::_nickNames.begin(), Client::_nickNames.end(), commandLine)) {
		_nickName = commandLine;
		Client::_nickNames.push_back(commandLine);
		std::cout << commandLine << " set for client" << std::endl;
	}
	else
		throw std::logic_error("433 " + _nickName + " " + commandLine + " :Nickname is already in use\r\n");
}

void Client::user(std::string &commandLine) {
	std::cout << "user" << std::endl;
}
