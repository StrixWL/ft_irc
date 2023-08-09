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
		if ((*it)->_nickName == commandLine) {
			alreadyExist = true;
			break ;
		}
	}
	if (!alreadyExist) {
		std::string oldNick = _nickName;
		_nickName = commandLine;
		logger.info("nickname to set: [" + commandLine + "]");
		if (oldNick != "*") {
			// broadcast nickname change to all clients
			for (std::vector<Client *>::iterator it = irc_server.all_clients.begin(); it != irc_server.all_clients.end(); it++)
				(*it)->send(":" + oldNick + " NICK " + _nickName + "\r\n");
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
	// only welcome once
	if (_welcomed)
		return ;
	_welcomed = true;
	_registered = true;
	// sending welcome message is required so that users can do stuff in the server
	send("001 " + _nickName + " :Welcome to the Strix Internet Relay Chat Network " + _nickName + "\r\n");
	logger.info("User \033[1m" + _nickName + "\033[0m authorized.");
}

void Client::join(std::string &commandLine) {
	// is used to know whether we are dealing with the first time join function is called for a commandline or not
	static bool isFirst = true;
	static std::string passwords = "";
	if (isFirst) {
		if (commandLine[0] == ':') { // + WE SURE THERE ARE NO PASSWORDS
			commandLine.erase(0, 1);
			passwords = "";
		}
		else { // passwords COULD exist
			if (commandLine.find(" ") != std::string::npos) {
				// all chars after the first " " are passwords, if there is more than 1 password they d be seperated by commas ","
				passwords = commandLine.substr(commandLine.find(" ") + 1, commandLine.length());
				if (passwords[0] == ':')
					passwords.erase(0, 1);
				commandLine = commandLine.substr(0, commandLine.find(" "));
			} 
			else
				passwords = "";
		}
	}
	// handle multiple channels instead of just one using recursion
	while (commandLine.find(",") != std::string::npos) {
		std::string nextChannel = commandLine.substr(0, commandLine.find(","));
		// we set it to false, this time when we call join it will know that it is not being called for the first time in this specific commandline
		isFirst = false;
		join(nextChannel);
		// we set it again to true so that the it will be true when we call it on the next commandline for the first time
		isFirst = true;
		commandLine = commandLine.erase(0, commandLine.find(",") + 1);
	}
	// inside try catch block so that we dont interrupt joining the remaining channels
	try {
		// channel names MUST not have white spaces
		if (commandLine.find(" ") != std::string::npos)
			logger.warn("476 " + _userName + " " + commandLine + " :Invalid channel name");
		// channel names MUST start with #
		if (commandLine[0] != '#') {
			logger.warn("403 " + _userName + " " + commandLine + " :No such channel");
	}
	} catch(std::exception &e) {
		send(e.what());
		return ;
	}
	// we check if channel already exist, if it does, add user to the channel, if not, create new channel
	for (std::vector<Channel *>::iterator it = irc_server.channels.begin(); it != irc_server.channels.end(); it++) {
		if ((*it)->_name == commandLine) {
			// i legit have no clue what am i doing at this point
			if (std::find((*it)->_members.begin(), (*it)->_members.end(), this) != (*it)->_members.end()) {
				// do nothing if member already part of the channel
				return ;
			}
			// take one password from the passwords string
			std::string enteredPW = passwords.substr(0, passwords.find(","));
			passwords.erase(0, enteredPW.length() + 1);
			try {
				// check if pw match, if not, return wrong pw
				if (enteredPW != (*it)->_password)
					logger.warn("475 " + _nickName + " " + commandLine + " :Cannot join channel (+k) - bad key");
			} catch (std::exception &e) {
				send(e.what());
				return ;
			}
			// add user to existing channel members
			(*it)->_members.push_back(this);
			// inform everyone that user has joined
			(*it)->broadcast(":" + _nickName + "!~" + _userName + "@" + _IPAddress + " JOIN " + commandLine + "\r\n");
			// send list of all existing members in that channel
			send("353 " + _nickName + " = " + (*it)->_name + " :" + (*it)->getNames() + "\r\n");
			send("366 " + _nickName + " " + (*it)->_name + " :End of /NAMES list.\r\n");
			return ;
		}
	}
	// creating new channel since it doesnt exist already
	Channel *newChannel = new Channel(commandLine);
	irc_server.channels.push_back(newChannel);
	// add user to channel members + operators
	newChannel->_members.push_back(this);
	newChannel->_operators.push_back(this);
	// inform everyone that user has joined
	newChannel->broadcast(":" + _nickName + "!~" + _userName + "@" + _IPAddress + " JOIN " + commandLine + "\r\n");
	// send list of all existing members in that channel
	send("353 " + _nickName + " = " + newChannel->_name + " :" + newChannel->getNames() + "\r\n");
	send("366 " + _nickName + " " + newChannel->_name + " :End of /NAMES list.\r\n");
}

void Client::privmsg(std::string &commandLine) {
	bool onlyOP = false;
	bool isChannel;
	// only operators will be able to see :O
	if (commandLine[0] == '@') {
		// to handle op only messages
		onlyOP = true;
		commandLine.erase(0, 1);
	}
	// channel stuff
	if (commandLine[0] == '#') {
		// find channel
		for (std::vector<Channel *>::iterator it = irc_server.channels.begin(); it != irc_server.channels.end(); it++) {
			// parse channel and message that we should broadcast
			std::string channel = commandLine.substr(0, commandLine.find(" "));
			std::string message = commandLine;
			message.erase(0, channel.length());
			while (message[0] == ' ')
				message.erase(0, 1);
			if ((*it)->_name == channel) {
				// channel found
				if (std::find((*it)->_members.begin(), (*it)->_members.end(), this) != (*it)->_members.end())
					// user is member of that channel
					if (onlyOP) // broadcast to only channel operators instead of everyone
						(*it)->broadcastOP(":" + _nickName + "!~" + _userName + "@" + _IPAddress + " PRIVMSG " + channel + " " + message + "\r\n", this);
					else
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
		logger.warn("401 " + _nickName + " " + receiver + " :No such nick");
	}
}

void Client::part(std::string &commandLine) {
	// is used to know whether we are dealing with the first time join function is called for a commandline or not
	static bool isFirst = true;
	static std::string leaveMessage = "";
	if (isFirst) {
		if (commandLine[0] == ':') // + WE SURE HERE THERE IS NO LEAVING MESSAGE
			commandLine.erase(0, 1);
		else { // leaving message COULD exist and its everything after the first " "
			if (commandLine.find(" ") != std::string::npos) {
				leaveMessage = commandLine.substr(commandLine.find(" ") + 1, commandLine.length());
			}
			commandLine = commandLine.substr(0, commandLine.find(" "));
		}
	}
	// handle multiple channels instead of just one using recursion
	while (commandLine.find(",") != std::string::npos) {
		std::string nextChannel = commandLine.substr(0, commandLine.find(","));
		// we set it to false, this time when we call join it will know that it is not being called for the first time in this specific commandline
		isFirst = false;
		part(nextChannel);
		// we set it again to true so that the it will be true when we call it on the next commandline for the first time
		isFirst = true;
		commandLine = commandLine.erase(0, commandLine.find(",") + 1);
	}
	try {
		// channel names MUST not have white spaces
		if (commandLine.find(" ") != std::string::npos)
			logger.warn("476 " + _userName + " " + commandLine + " :Invalid channel name");
		// channel names MUST start with #
		if (commandLine[0] != '#') {
			logger.warn("403 " + _userName + " " + commandLine + " :No such channel");
	}
	} catch(std::exception &e) {
		send(e.what());
		return ;
	}
	// we check if channel already exist, if it does, add user to the channel, if not, create new channel
	for (std::vector<Channel *>::iterator it = irc_server.channels.begin(); it != irc_server.channels.end(); it++) {
		if ((*it)->_name == commandLine) {
			// i legit have no clue what am i doing at this point
			if (std::find((*it)->_members.begin(), (*it)->_members.end(), this) != (*it)->_members.end()) {
				// IS PART OF THE CHANNEL
				// erase
				(*it)->broadcast(":" + _nickName + "!~" + _userName + "@" + _IPAddress + " PART " + commandLine + " " + leaveMessage + "\r\n");
				// the first function execution will execute the last chained channel, means this part will be executed the last, we use it to reset our leaveMessage cuz it is static!
				if (isFirst)
					leaveMessage = "";
				(*it)->_members.erase(std::find((*it)->_members.begin(), (*it)->_members.end(), this));
				if (!(*it)->_members.size()) {
					delete *it;
					irc_server.channels.erase(it);
				}
				return ;
			}
			logger.warn("403 " + _userName + " " + commandLine + " :No such channel");
		}
	}
	logger.warn("403 " + _userName + " " + commandLine + " :No such channel");
}

void Client::quit(std::string &commandLine) {
	if (commandLine[0] == ':')
		commandLine.erase(0, 1);
	_leaveMessage = commandLine;
	send("ERROR :Closing Link: " + _IPAddress + " (Client Quit)\r\n");
	_keepAlive = false;
}

// brojola hadshi tl3li frassi hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhelpmepleasehhhhhhhhhhhhhhhhhhhhhh