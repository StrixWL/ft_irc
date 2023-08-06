#include "Client.hpp"
#include <algorithm>
#include <Server.hpp>
#include "Channel.hpp"

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
		welcome();
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
		welcome();
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
	logger.verbose("username set to " + _userName);
	_realName = realName;
	logger.verbose("realname set to " + _realName);
	// we dont knoe which auth command will be executed the last so everytime we check whether we have the conditions
		// to authorize the client or not
	if (_userName.length() && _realName.length() && _nickName != "*" && _authorized)
		welcome();
}

void Client::welcome(void) {
	_registered = true;
	// sending welcome message is required so that users can do stuff in the server
	send("001 " + _nickName + " :Welcome to the Strix Internet Relay Chat Network " + _nickName + "\r\n");
	logger.info("User \033[1m" + _nickName + "\033[0m authorized.");
}

void Client::join(std::string &commandLine) {
	// BIGASS TODO: handle multiple channels instead of just one
	if (commandLine[0] != '#')
		logger.warn("403 " + _userName + " " + commandLine + " :No such channel");
	// we check if channel already exist, if it does, add user to the channel, if not, create new channel
	for (std::vector<Channel *>::iterator it = irc_server.channels.begin(); it != irc_server.channels.end(); it++) {
		if ((*it)->_name == commandLine) {
			// i legit have no clue what am i doing at this point
			if (std::find((*it)->_members.begin(), (*it)->_members.end(), this) != (*it)->_members.end()) {
				// do nothing if member already part of the channel
				return ;
			}
			// add user to channel members
			(*it)->_members.push_back(this);
			// inform everyone that user has joined
			(*it)->broadcast(":" + _nickName + "!~" + _userName + "@" + _IPAddress + " JOIN " + commandLine + "\r\n");
			// TODO: send _userName list of all existing members in that channel
			return ;
		}
	}
	// creating new channel since it doesnt exist already
	Channel *newChannel = new Channel(commandLine);
	irc_server.channels.push_back(newChannel);
	// add user to channel members
	newChannel->_members.push_back(this);
	// inform everyone that user has joined
	newChannel->broadcast(":" + _nickName + "!~" + _userName + "@" + _IPAddress + " JOIN " + commandLine + "\r\n");
	// TODOn't: send _userName list of all existing members in that channel, cuz he's the first one to join so no need lol
	// TODO: inform user that he's the channel operator
}

void Client::privmsg(std::string &commandLine) {
	bool onlyOP = false;
	bool isChannel;
	// only operators will be able to see :O
	if (commandLine[0] == '@') {
		// TODO: handle op only messages
		onlyOP = true;
		commandLine.erase(0, 1);
	}
	// channel stuff
	if (commandLine[0] == '#') {
		// find channel
		for (std::vector<Channel *>::iterator it = irc_server.channels.begin(); it != irc_server.channels.end(); it++) {
			// parse channel and message that we should broadcast
			std::string channel = commandLine.substr(0, commandLine.find(" "));
			commandLine.erase(0, channel.length());
			while (commandLine[0] == ' ')
				commandLine.erase(0, 1);
			std::string message = commandLine;
			if ((*it)->_name == channel) {
				// channel found
				if (std::find((*it)->_members.begin(), (*it)->_members.end(), this) != (*it)->_members.end())
					// user is member of that channel
					(*it)->broadcast(":" + _nickName + "!~" + _userName + "@" + _IPAddress + " PRIVMSG " + channel + " " + message + "\r\n", this);
				else
					send("404 " + _nickName + " " + channel + " :Cannot send to nick/channel\r\n");
				return ;
			}
		}
		logger.warn("403 " + _userName + " " + commandLine + " :No such channel");
	}
	// DM stuff
	else {
		std::string receiver = commandLine.substr(0, commandLine.find(" "));
		commandLine.erase(0, receiver.length());
		while (commandLine[0] == ' ')
			commandLine.erase(0, 1);
		std::string message = commandLine;
		// loop through all clients, if not found, return 401
		for (std::vector<Client *>::iterator it = irc_server.all_clients.begin(); it != irc_server.all_clients.end(); it++) {
			if ((*it)->_nickName == receiver) {
				// :nickname PRIVMSG receiver :message
				(*it)->send(":" + _nickName + " PRIVMSG " + receiver + " :" + message + "\r\n");
				return ;
			}
		}
		logger.warn("401 " + _nickName + " " + receiver + " :No such nick/channel");
	}
}

void Client::quit(std::string &commandLine) {
	send("ERROR :Closing Link: " + _IPAddress + " (Client Quit)\r\n");
	_keepAlive = false;
}
