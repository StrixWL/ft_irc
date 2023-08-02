#include "Client.hpp"
#include <algorithm>
#include <Server.hpp>

void Client::pass(std::string &commandLine) { // 100% finished
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
	// we dont knoe which auth command will be executed the last so everytime we check whether we have the conditions
		// to authorize the client or not
	if (_userName.length() && _realName.length() && _nickName != "*" && _authorized)
		_registered = true;
}

void Client::nick(std::string &commandLine) { // 100% finished
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
	bool alreadyExist = false;
	for (std::vector<Client *>::iterator it = irc_server.all_clients.begin(); it != irc_server.all_clients.end(); it++) {
		if ((*it)->_nickName == commandLine)
			alreadyExist = true;
	}
	if (!alreadyExist) {
		std::string oldNick = _nickName;
		_nickName = commandLine;
		logger.info("nickname to set: [" + commandLine + "]");
		if (oldNick != "*") {
			// broadcast nickname change to all clients
			for (std::vector<Client *>::iterator it = irc_server.all_clients.begin(); it != irc_server.all_clients.end(); it++) {
				(*it)->send(":" + oldNick + " NICK " + _nickName + "\r\n");
			}
		}
	}
	else
		logger.warn("433 " + _nickName + " " + commandLine + " :Nickname is already in use");
	// we dont knoe which auth command will be executed the last so everytime we check whether we have the conditions
		// to authorize the client or not
	if (_userName.length() && _realName.length() && _nickName != "*" && _authorized)
		_registered = true;
}

void Client::user(std::string &commandLine) { // 100% finished
	// just simple repetitive parsing, we dont need the 2nd and 3rd arg, 1st is username and 4th is realname
	std::string userName = commandLine.substr(0, commandLine.find(" "));
	commandLine.erase(0, userName.length());
	while (commandLine[0] == ' ')
		commandLine.erase(0, 1);
	std::string zero = commandLine.substr(0, commandLine.find(" "));
	commandLine.erase(0, zero.length());
	while (commandLine[0] == ' ')
		commandLine.erase(0, 1);
	std::string asterisk = commandLine.substr(0, commandLine.find(" ")); // asterisk is the character '*'
	commandLine.erase(0, asterisk.length());
	while (commandLine[0] == ' ')
		commandLine.erase(0, 1);
	std::string realName = commandLine;
	if (!zero.length() || !asterisk.length() || !userName.length() || !realName.length())
		logger.warn("461 " + _nickName + " " + commandLine + " :Not enough parameters");
	_userName = userName;
	logger.info("username set to " + _userName);
	_realName = realName;
	logger.info("realname set to " + _realName);
	// we dont knoe which auth command will be executed the last so everytime we check whether we have the conditions
		// to authorize the client or not
	if (_userName.length() && _realName.length() && _nickName != "*" && _authorized)
		_registered = true;
}
