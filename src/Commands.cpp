#include "Client.hpp"
#include <algorithm>

void Client::pass(std::string &commandLine) {
	// no params
	if (!commandLine.length())
		logger.warn("461 " + _nickName + " :Not enough parameters");
	// cringe ":" took me an hour to debug
	if (commandLine[0] == ':')
		commandLine.erase(0, 1);
	_password = commandLine;
	logger.info("password set to : [" + commandLine + "]");
	_authorized = true;
	// comparing outputted pw with server pw
	if (commandLine != PASSWORD) {
		// nn hh
		_authorized = false;
		std::string e = "464 " + _nickName + " :Password incorrect";
		logger.warn(e);
	}
}

void Client::nick(std::string &commandLine) {
	// not this time 
	if (commandLine[0] == ':')
		commandLine.erase(0, 1);
	else // take all characters until space
		commandLine = commandLine.substr(0, commandLine.find(" "));
	// no params
	if (!commandLine.length())
		logger.warn("431 " + _nickName + " :No nickname given");
	// check for forbidden chars
	for (std::string::iterator it = commandLine.begin(); it != commandLine.end(); it++) {
		if (std::string("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-[]\\`^{}_|").find(*it) == std::string::npos)
			logger.warn("432 " + _nickName + " " + commandLine + " :Erroneus nickname");
	}
	//check if nickname alredy exist
	if (!std::count(Client::_nickNames.begin(), Client::_nickNames.end(), commandLine)) {
		std::string oldNick = _nickName;
		_nickName = commandLine;
		Client::_nickNames.push_back(commandLine);
		logger.info("nickname to set: [" + commandLine + "]");
		if (oldNick != "*") {
			// delete old nick
			Client::_nickNames.erase(std::find(Client::_nickNames.begin(), Client::_nickNames.end(), oldNick));
			// TODO: broadcast nickname change to all users
			std::cout << "oldnick: " << oldNick << std::endl;
			std::cout << "newnick: " << _nickName << std::endl;
			send(":" + oldNick + " NICK " + _nickName + "\r\n");
		}
	}
	else
		logger.warn("433 " + _nickName + " " + commandLine + " :Nickname is already in use");
}

void Client::user(std::string &commandLine) {
}

