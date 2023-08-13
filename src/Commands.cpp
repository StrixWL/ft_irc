#include "Client.hpp"
#include <algorithm>
#include <Server.hpp>
#include "Channel.hpp"

std::vector<std::string> getArgs(std::string commandLine) {
	std::vector<std::string> args;
	while (commandLine.length()) {
		std::string nextArg;
		// if ':' is found, take the remaining chars of the string as one argument
		if (commandLine[0] == ':') {
			commandLine.erase(0, 1);
			nextArg = commandLine;
		}
		// else take until whitespace
		else
			nextArg = commandLine.substr(0, commandLine.find(" "));
		// remove argument taken from the command line
		commandLine.erase(0, nextArg.length());
		// skip all spaces
		while (commandLine[0] == ' ')
			commandLine.erase(0, 1);
		// skip arg if empty
		if (nextArg.length())
			args.push_back(nextArg);
	}
	return args;
}

void Client::pass(std::string &commandLine) {
	std::vector<std::string> args = getArgs(commandLine);
	// no params
	if (!args.size())
		logger.warn("461 " + _nickName + " :Not enough parameters");
	_password = args[0];
	logger.info("password set to : [" + _password + "]");
	_authorized = true;
	// comparing outputted pw with server pw
	if (_password != irc_server.password) {
		// nn hh
		_authorized = false;
		std::string e = "464 " + _nickName + " :Password incorrect";
		logger.warn(e);
	}
	/* we dont knoe which auth command will be executed the last so everytime we check whether we have the conditions
		to authorize the client or not */
	if (_userName.length() && _realName.length() && _nickName != "*" && _authorized)
		welcome();
}

void Client::nick(std::string &commandLine) {
	std::vector<std::string> args = getArgs(commandLine);
	// no params
	if (!args.size())
		logger.warn("431 " + _nickName + " :No nickname given");
	// check for forbidden chars
	for (std::string::iterator it = args[0].begin(); it != args[0].end(); it++) {
		if (std::string("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-[]\\`^{}_|").find(*it) == std::string::npos)
			logger.warn("432 " + _nickName + " " + args[0] + " :Erroneus nickname");
	}
	//check if nickname alredy exist
	bool alreadyExist = false;
	for (std::vector<Client *>::iterator it = irc_server.all_clients.begin(); it != irc_server.all_clients.end(); it++) {
		if ((*it)->_nickName == args[0]) {
			alreadyExist = true;
			break ;
		}
	}
	if (!alreadyExist) {
		std::string oldNick = _nickName;
		_nickName = args[0];
		logger.info("nickname to set: [" + args[0] + "]");
		if (oldNick != "*") {
			// broadcast nickname change to all clients
			for (std::vector<Client *>::iterator it = irc_server.all_clients.begin(); it != irc_server.all_clients.end(); it++)
				(*it)->send(":" + oldNick + " NICK " + _nickName + "\r\n");
		}
	}
	else
		logger.warn("433 " + _nickName + " " + args[0] + " :Nickname is already in use");
	/* we dont knoe which auth command will be executed the last so everytime we check whether we have the conditions
		to authorize the client or not */
	if (_userName.length() && _realName.length() && _nickName != "*" && _authorized)
		welcome();
}

void Client::user(std::string &commandLine) {
	std::vector<std::string> args = getArgs(commandLine);
	// check for params
	if (args.size() != 4)
		logger.warn("461 " + _nickName + " " + commandLine + " :Not enough parameters");
	std::string userName = args[0];
	std::string zero = args[1];
	std::string asterisk = args[2]; // asterisk = the character '*'
	std::string realName = args[3];
	// we dont need zero and asterisk, doenst matter what their value is
	_userName = userName;
	logger.verbose("username set to " + _userName);
	_realName = realName;
	logger.verbose("realname set to " + _realName);
	/* we dont knoe which auth command will be executed the last so everytime we check whether we have the conditions
		to authorize the client or not */
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
	send("001 " + _nickName + " :Welcome to our Internet Relay Chat Network " + _nickName + " ^_^\r\n");
	logger.info("User \033[1m" + _nickName + "\033[0m authorized.");
}

void Client::join(std::string &commandLine) {
	std::vector<std::string> args = getArgs(commandLine);
	// no params
	if (!args.size())
		logger.warn("461 " + _nickName + " JOIN :Not enough parameters");
	std::string channels = args[0];
	std::string passwords = (args.size() >= 2) ? args[1] : "";
	// loop used to go through all chained channels
	while (channels.find(",") != std::string::npos) {
		// we parse params from our args to join 1 channel
		std::string nextChannel = channels.substr(0, channels.find(","));
		channels.erase(0, nextChannel.length() + 1);
		std::string nextPassword = passwords.substr(0, passwords.find(","));
		passwords.erase(0, nextPassword.length() + 1);
		std::string nextCall = nextChannel + " " + nextPassword;
		// we call join again with our new params
		join(nextCall);
	}
	// inside try catch block so that we dont interrupt joining the remaining channels
	try {
		// channel names MUST not have white spaces
		if (channels.find(" ") != std::string::npos)
			logger.warn("476 " + _nickName + " " + channels + " :Invalid channel name");
		// channel names MUST start with #
		if (channels[0] != '#') {
			logger.warn("403 " + _nickName + " " + channels + " :No such channel");
	}
	} catch(std::exception &e) {
		send(e.what());
		return ;
	}
	// we check if channel already exist, if it does, add user to the channel, if not, create new channel
	for (std::vector<Channel *>::iterator it = irc_server.channels.begin(); it != irc_server.channels.end(); it++) {
		if ((*it)->_name == channels) {
			// i legit have no clue what am i doing at this point
			if (std::find((*it)->_members.begin(), (*it)->_members.end(), this) != (*it)->_members.end()) {
				// do nothing if member already part of the channel
				return ;
			}
			try {
				// check for key first, then invite
				// check if pw match, if not, return wrong pw
				if ((*it)->_password.length())
					if (passwords != (*it)->_password)
						logger.warn("475 " + _nickName + " " + channels + " :Cannot join channel (+k) - bad key");
				// check if channel is full
				if ((*it)->_limit && (*it)->_members.size() >= (*it)->_limit)
					logger.warn("471 " + _nickName + " " + channels + " :Cannot join channel (+l) - channel is full, try again later");
				// check if _inviteOnly is true, check if _nickName exists in invitees list
				if ((*it)->_inviteOnly && (std::find((*it)->_invitees.begin(), (*it)->_invitees.end(), this) == (*it)->_invitees.end()))
					logger.warn("473 " + _nickName + " " + channels + " :Cannot join channel (+i) - you must be invited");

			} catch (std::exception &e) {
				send(e.what());
				return ;
			}
			// add user to existing channel members
			(*it)->_members.push_back(this);
			if (std::find((*it)->_invitees.begin(), (*it)->_invitees.end(), this) != (*it)->_invitees.end())
				(*it)->_invitees.erase(std::find((*it)->_invitees.begin(), (*it)->_invitees.end(), this));
			// inform everyone that user has joined
			(*it)->broadcast(":" + _nickName + "!~" + _userName + "@" + _IPAddress + " JOIN " + channels + "\r\n");
			// send topic channel on join
			if ((*it)->_topic.length())
				send("332 " + _nickName + " " + (*it)->_name + " :" + (*it)->_topic + "\r\n");
			// send list of all existing members in that channel
			send("353 " + _nickName + " = " + (*it)->_name + " :" + (*it)->getNames() + "\r\n");
			send("366 " + _nickName + " " + (*it)->_name + " :End of /NAMES list.\r\n");
			return ;
		}
	}
	// creating new channel since it doesnt exist already
	Channel *newChannel = new Channel(channels);
	irc_server.channels.push_back(newChannel);
	// add user to channel members + operators
	newChannel->_members.push_back(this);
	// add to operators since the first person to join/create a channel becomes its op
	newChannel->_operators.push_back(this);
	// inform everyone that user has joined
	newChannel->broadcast(":" + _nickName + "!~" + _userName + "@" + _IPAddress + " JOIN " + channels + "\r\n");
	// send list of all existing members in that channel
	send("353 " + _nickName + " = " + newChannel->_name + " :" + newChannel->getNames() + "\r\n");
	send("366 " + _nickName + " " + newChannel->_name + " :End of /NAMES list.\r\n");
}

void Client::privmsg(std::string &commandLine) {
	std::vector<std::string> args = getArgs(commandLine);
	if (args.size() <= 1)
		return ;
	std::string channel = args[0];
	std::string message = args[1];
	// handle multiple targets, like JOIN
	while (channel.find(",") != std::string::npos) {
		std::string nextChannel = channel.substr(0, channel.find(","));
		channel.erase(0, nextChannel.length() + 1);
		std::string nextCall = nextChannel + " :" + message;
		privmsg(nextCall);
	}
	bool onlyOP = false;
	// only operators will be able to see :O
	if (channel[0] == '@') {
		// to handle op only messages
		onlyOP = true;
		channel.erase(0, 1);
	}
	// channel stuff
	if (channel[0] == '#') {
		// find channel
		for (std::vector<Channel *>::iterator it = irc_server.channels.begin(); it != irc_server.channels.end(); it++) {
			// parse channel and message that we should broadcast
			if ((*it)->_name == channel) {
				// channel found
				if (std::find((*it)->_members.begin(), (*it)->_members.end(), this) != (*it)->_members.end())
					// user is member of that channel
					if (onlyOP) // broadcast to only channel operators instead of everyone
						(*it)->broadcastOP(":" + _nickName + "!~" + _userName + "@" + _IPAddress + " PRIVMSG " + channel + " :" + message + "\r\n", this);
					else
						(*it)->broadcast(":" + _nickName + "!~" + _userName + "@" + _IPAddress + " PRIVMSG " + channel + " :" + message + "\r\n", this);
				else
					send("404 " + _nickName + " " + channel + " :Cannot send to nick/channel\r\n");
				return ;
			}
		}
		logger.warn("403 " + _nickName + " " + channel + " :No such channel");
	}
	// DM stuff
	else {
		// loop through all clients, if not found, return 401
		for (std::vector<Client *>::iterator it = irc_server.all_clients.begin(); it != irc_server.all_clients.end(); it++) {
			if ((*it)->_nickName == channel) {
				// :nickname PRIVMSG channel :message
				(*it)->send(":" + _nickName + " PRIVMSG " + channel + " :" + message + "\r\n");
				return ;
			}
		}
		logger.warn("401 " + _nickName + " " + channel + " :No such nick");
	}
}

void Client::part(std::string &commandLine) {
	std::vector<std::string> args = getArgs(commandLine);
	if (!args.size())
		logger.warn("461 " + _nickName + " PART :Not enough parameters");
	std::string channels = args[0];
	std::string leaveMessage = (args.size() >= 2) ? args[1] : "";
	// to handle multiple channels, like JOIN
	while (channels.find(",") != std::string::npos) {
		std::string nextChannel = channels.substr(0, channels.find(","));
		channels.erase(0, nextChannel.length() + 1);
		std::string nextCall = nextChannel + " " + leaveMessage;
		part(nextCall);
	}
	try {
		// channel names MUST not have white spaces
		if (channels.find(" ") != std::string::npos)
			logger.warn("476 " + _nickName + " " + channels + " :Invalid channel name");
		// channel names MUST start with #
		if (channels[0] != '#')
			logger.warn("403 " + _nickName + " " + channels + " :No such channel");
		for (std::vector<Channel *>::iterator it = irc_server.channels.begin(); it != irc_server.channels.end(); it++) {
			if ((*it)->_name == channels) {
				// i legit have no clue what am i doing at this point
				if (std::find((*it)->_members.begin(), (*it)->_members.end(), this) != (*it)->_members.end()) {
					// IS PART OF THE CHANNEL
					(*it)->broadcast(":" + _nickName + "!~" + _userName + "@" + _IPAddress + " PART " + channels + " " + leaveMessage + "\r\n");
					(*it)->_members.erase(std::find((*it)->_members.begin(), (*it)->_members.end(), this));
					// remove from _operators if exist
					if (std::find((*it)->_operators.begin(), (*it)->_operators.end(), this) != (*it)->_operators.end())
						(*it)->_operators.erase(std::find((*it)->_operators.begin(), (*it)->_operators.end(), this));
					if (!(*it)->_members.size()) {
						// channel empty
						delete *it;
						irc_server.channels.erase(it);
					}
					return ;
				}
				logger.warn("403 " + _nickName + " " + channels + " :No such channel");
			}
		}	
		logger.warn("403 " + _nickName + " " + channels + " :No such channel");
	} catch(std::exception &e) {
		send(e.what());
		return ;
	}
}

void Client::kick(std::string &commandLine) {
	std::vector<std::string> args = getArgs(commandLine);
	if (args.size() < 2)
		logger.warn("461 " + _nickName + " :KICK Not enough parameters");
	std::string channel = args[0];
	std::string users = args[1];
	// BTW users could be multiple users and could be only 1 user as well, its always 1 user while in recursive execution
	std::string reason = (args.size() >= 3) ? args[2] : "";
	// to handle multiple users, like JOIN
	while (users.find(",") != std::string::npos) {
		std::string nextUser = users.substr(0, users.find(","));
		users.erase(0, nextUser.length() + 1);
		std::string nextCall = channel + " " + nextUser + " " + reason;
		kick(nextCall);
	}
	for (std::vector<Channel *>::iterator it = irc_server.channels.begin(); it != irc_server.channels.end(); it++) {
		if ((*it)->_name == channel) {
			// channel found
			// now we check if user is in channel
			if (std::find((*it)->_members.begin(), (*it)->_members.end(), this) == (*it)->_members.end())
				logger.warn("442 " + _nickName + " " + channel + " :You're not on that channel");			
			// now we check if user has privilege
			if (std::find((*it)->_operators.begin(), (*it)->_operators.end(), this) == (*it)->_operators.end())
				logger.warn("482 " + _nickName + " " + channel + " :You're not a channel operator");
			for (std::vector<Client *>::iterator _it = (*it)->_operators.begin(); _it != (*it)->_operators.end(); _it++) {
				if ((*_it)->_nickName == users) {
					// operator found
					(*it)->_operators.erase(_it);
					break ;
				}
			}
			for (std::vector<Client *>::iterator _it = (*it)->_members.begin(); _it != (*it)->_members.end(); _it++) {
				if ((*_it)->_nickName == users) {
					// user found
					// broadcast kick message to all members
					(*it)->broadcast(":" + _nickName + "!~" + _userName + "@" + _IPAddress + " KICK " + channel + " " + users + " :" + reason + "\r\n");
					(*it)->_members.erase(_it);
					if (!(*it)->_members.size()) {
						// channel empty
						delete (*it);
						irc_server.channels.erase(it);
					}
					return ;
				}
			}
			logger.warn("401 " + _nickName + " " + users + " :No such nick/channel");
		}
	}
	logger.warn("403 " + _nickName + " " + channel + " :No such channel");
}

void Client::invite(std::string &commandLine) {
	std::vector<std::string> args = getArgs(commandLine);
	if (args.size() < 2)
		logger.warn("461 " + _nickName + " :Not enough parameters");
	std::string invitee = args[0];
	std::string channel = args[1];
	Client *inviteeObj = NULL;
	Channel *channelObj = NULL;
	// check if user exist
	for (std::vector<Client *>::iterator it = irc_server.all_clients.begin(); it != irc_server.all_clients.end(); it++) {
		if ((*it)->_nickName == invitee) {
			// user found
			inviteeObj = *it;
			break ;
		}
	}
	if (!inviteeObj)
		logger.warn("401 " + _nickName + " " + invitee + " :No such nick/channel");
	// check if channel exist
	for (std::vector<Channel *>::iterator it = irc_server.channels.begin(); it != irc_server.channels.end(); it++) {
		if ((*it)->_name == channel) {
			// channel found
			channelObj = *it;
			break ;
		}
	}
	if (!channelObj)
		logger.warn("403 " + _nickName + " " + channel + " :No such channel");
	// check if inviter is not in channel
	if (std::find(channelObj->_members.begin(), channelObj->_members.end(), this) == channelObj->_members.end())
		logger.warn("442 " + _nickName + " " + channel + " :You're not on that channel");
	// check if invitee is in channel already
	if (std::find(channelObj->_members.begin(), channelObj->_members.end(), inviteeObj) != channelObj->_members.end())
		logger.warn("443 " + _nickName + " " + invitee + " " + channel + " :is already on channel");
	// add invitee to channel invitees list
	channelObj->_invitees.push_back(inviteeObj);
	// inform invitee he has been invited
	inviteeObj->send(":" + _nickName + "!~" + _userName + "@" + _IPAddress + " INVITE " + invitee + " :" + channel + "\r\n");
	// inform inviter he invited invitee
	send("341 " + _nickName + " " + invitee + " " + channel + "\r\n");
}

void Client::topic(std::string &commandLine) {
	std::vector<std::string> args = getArgs(commandLine);
	if (!args.size())
		logger.warn("461 " + _nickName + " :Not enough parameters");
	std::string channel = args[0];
	std::string topic = args.size() >= 2 ? args[1] : "";
	Channel *channelObj = NULL;
	for (std::vector<Channel *>::iterator it = irc_server.channels.begin(); it != irc_server.channels.end(); it++) {
		if ((*it)->_name == channel) {
			// channel found
			channelObj = *it;
			break ;
		}
	}
	if (!channelObj)
		logger.warn("403 " + _nickName + " " + channel + " :No such channel");
	if (std::find(channelObj->_members.begin(), channelObj->_members.end(), this) == channelObj->_members.end())
		logger.warn("442 " + _nickName + " " + channel + " :You're not on that channel");
	// in case there is no topic param, just return existing one;
	if (!topic.size()) {
		// send topic
		if (!channelObj->_topic.length())
			send("331 " + _nickName + " " + channel + " :" + "No topic is set." + "\r\n");
		else
			send("332 " + _nickName + " " + channel + " :" + channelObj->_topic + "\r\n");
		return ;
	}
	// if only op can change topic, check if client is op
	if (channelObj->_opOnlyTopic)
		if (std::find(channelObj->_operators.begin(), channelObj->_operators.end(), this) == channelObj->_operators.end())
			logger.warn("482 " + _nickName + " " + channel + " :You're not a channel operator");
	// set topic
	channelObj->_topic = topic;
	// let everyoen know topic changed
	channelObj->broadcast(":" + _nickName + "!~" + _userName + "@" + _IPAddress + " TOPIC " + channel + " :" + topic + "\r\n");
}

// soon :3
void Client::mode(std::string &commandLine) {
	std::vector<std::string> args = getArgs(commandLine);
	if (!args.size())
		logger.warn("461 " + _nickName + " :MODE Not enough parameters");
	std::string channel = args[0];
	Channel *channelObj = NULL;
	for (std::vector<Channel *>::iterator it = irc_server.channels.begin(); it != irc_server.channels.end(); it++) {
		if ((*it)->_name == channel) {
			// channel found
			channelObj = *it;
			break ;
		}
	}
	if (!channelObj)
		logger.warn("403 " + _nickName + " " + channel + " :No such channel");
	if (args.size() == 1) {
		// sending channel modes
		std::string modes = channelObj->getModes();
		if (modes.length() != 1)
			send("324 " + _nickName + " " + channel + " :" + channelObj->getModes() + "\r\n");
		return ;
	}
	std::string modestring = args[1];
	// remove channel name and modestring
	args.erase(args.begin());
	args.erase(args.begin());
	// now we got only mode arguments left in args
	std::string _modestring = "";
	std::string chunk = "";
	char sign = modestring[0] == '-' ? '-' : '+';
	for (int i = 0; i < modestring.length(); i++) {
		if (modestring[i] == '-' || modestring[i] == '+') {
			if (sign != modestring[i]) {
				_modestring = _modestring + sign + chunk;
				sign = modestring[i];
				chunk = "";
			}
		}
		else {
			try {
				// check if mode is recognised
				if (std::string("itkol").find(modestring[i]) == std::string::npos)
					logger.warn("472 " + _nickName + " " + modestring[i] + " :is not a recognised channel mode.");
			}
			catch(std::exception &e) {
				send(e.what());
				continue  ;
			}
			// if mode isnt already added, add it
			if (chunk.find(modestring[i]) == std::string::npos) {
				chunk += modestring[i];
			}
		}
	}
	_modestring = _modestring + sign + chunk;
	// remove unecessary signs from _modestring
	for (int i = 0; i < _modestring.length(); i++) {
		while ((_modestring[i] == '-' || _modestring[i] == '+') && (_modestring[i + 1] == '-' || _modestring[i + 1] == '+'))
			_modestring.erase(i, 1);
	}
	if (_modestring[_modestring.length() - 1] == '-' || _modestring[_modestring.length() - 1] == '+')
		_modestring.erase(_modestring.length() - 1, 1);
	std::string finalModestring = "";
	std::string finalArgs = "";
	bool _sign;
	for (int i = 0; i < _modestring.length(); i++) {
		if (i - 1 && (_modestring[i - 1] == 'k' || _modestring[i - 1] == 'o' || _modestring[i - 1] == 'l'))
			args.erase(args.begin());
		try {
			if (_modestring[i] == '-') {
				finalModestring += '-';
				_sign = false;
			}
			else if (_modestring[i] == '+') {
				finalModestring += '+';
				_sign = true;
			}
			else {
				// check if client is OP (we check for access rights inside of each odmmande)
				if (std::find(channelObj->_operators.begin(), channelObj->_operators.end(), this) == channelObj->_operators.end())
					logger.warn("482 " + _nickName + " " + channel +  " You must have channel op access to set channel mode " + _modestring[i]);
				if (_modestring[i] == 'i') {
					if (channelObj->_inviteOnly != _sign)
						finalModestring += 'i';
					channelObj->_inviteOnly = _sign;
				}
				else if (_modestring[i] == 't') {
					if (channelObj->_opOnlyTopic != _sign)
						finalModestring += 't';
					channelObj->_opOnlyTopic = _sign;
				}
				else if (_modestring[i] == 'k') {
					// set channel pw
					if (args.size()) {
						if (args[0].find(" ") != std::string::npos) {
							logger.warn("696 " + _nickName + " " + channel +  " k * :Invalid key mode parameter. Syntax: <key>.");
						}
						// we can set pw
						std::string oldPw = channelObj->_password;
						if (!_sign)
							channelObj->_password = "";
						else
							channelObj->_password = args[0];
						if (oldPw != channelObj->_password) {
							finalArgs += " " + args[0];
							finalModestring += 'k';
						}
					}
					else {
						// we cant set pw
						logger.warn("696 " + _nickName + " " + channel + " k * You must specify a parameter for the key mode. Syntax: <key>.");
					}
				}
				else if (_modestring[i] == 'o') {
					// set channel op
					if (args.size()) {
						if (args[0].find(" ") != std::string::npos) {
							logger.warn("696 " + _nickName + " " + channel +  " o * Invalid op mode parameter. Syntax: <nick>.");
						}
						// check if args[0] (user) exist in channel members
						Client *newOpObj = NULL;
						for (std::vector<Client *>::iterator it = channelObj->_members.begin(); it != channelObj->_members.end(); it++) {
							if ((*it)->_nickName == args[0]) {
								newOpObj = *it;
								break ;
							}
						}
						if (!newOpObj)
							logger.warn("401 " + _nickName + " " + args[0] +  " No such nick");
						// we can set op
						if (!_sign) {
							for (std::vector<Client *>::iterator it = channelObj->_operators.begin(); it != channelObj->_operators.end(); it++) {
								if ((*it)->_nickName == args[0]) {
									channelObj->_operators.erase(it);
									finalArgs += " " + args[0];
									finalModestring += 'o';
									break ;
								}
							}
						}
						else {
							bool alreadyExist = false;
							for (std::vector<Client *>::iterator it = channelObj->_operators.begin(); it != channelObj->_operators.end(); it++) {
								// break if there is already an op with the same nickname
								if ((*it)->_nickName == args[0]) {
									alreadyExist = true;
									break ;
								}
							}
							if (!alreadyExist) {
								finalArgs += " " + args[0];
								finalModestring += 'o';
								channelObj->_operators.push_back(newOpObj);
							}
						}
					}
					else {
						// we cant set op
						logger.warn("696 " + _nickName + " " + channel + " o * You must specify a parameter for the op mode. Syntax: <nick>.");
					}
				}
				else if (_modestring[i] == 'l') {
					// set channel limit
					if (args.size()) {
						if (args[0].find(" ") != std::string::npos) { 
							logger.warn("696 " + _nickName + " " + channel +  " l * Invalid limit mode parameter. Syntax: <limit>.");
						}
						// should not equal to 0 and start with a number
						int limitToSet;
						try {
							limitToSet = std::stoi(args[0]);
							if (!limitToSet)
								throw std::logic_error("salam sahbi hh");
						} catch (std::exception &e) {
							logger.warn("696 " + _nickName + " " + channel +  " l " + args[0] + " Invalid limit mode parameter. Syntax: <limit>.");
						}
						// we can set channel limit
						int oldLimit = channelObj->_limit;
						if (!_sign)
							channelObj->_limit = 0;
						else
							channelObj->_limit = limitToSet;
						if (channelObj->_limit != oldLimit) {
							finalArgs += " " + std::to_string(channelObj->_limit);
							finalModestring += 'l';
						}
					}
					else {
						// we cant set pw
						logger.warn("696 " + _nickName + " " + channel + " l * You must specify a parameter for the limit mode. Syntax: <limit>.");
					}
				}
			}
		} catch (std::exception &e) {
			send(e.what());
		}
	}
	// remove unecessary signs from finalModestring
	for (int i = 0; i < finalModestring.length(); i++) {
		while ((finalModestring[i] == '-' || finalModestring[i] == '+') && (finalModestring[i + 1] == '-' || finalModestring[i + 1] == '+'))
			finalModestring.erase(i, 1);
	}
	if (finalModestring[finalModestring.length() - 1] == '-' || finalModestring[finalModestring.length() - 1] == '+')
		finalModestring.erase(finalModestring.length() - 1, 1);
	if (finalModestring.length())
		channelObj->broadcast(":" + _nickName + "!~" + _userName + "@" + _IPAddress + " MODE " + channel + " " + finalModestring + finalArgs + "\r\n");
}

void Client::names(std::string &commandLine) {
	std::vector<std::string> args = getArgs(commandLine);
	if (args.size()) {
		for (std::vector<Channel *>::iterator it = irc_server.channels.begin(); it != irc_server.channels.end(); it++) {
			if ((*it)->_name == args[0]) {
				if (std::find((*it)->_members.begin(), (*it)->_members.end(), this) != (*it)->_members.end())
					send("353 " + _nickName + " = " + (*it)->_name + " :" + (*it)->getNames() + "\r\n");
				break ;
			}
		}
	}
	send("366 " + _nickName + " " + (args.size() ? args[0] : "*") + " :End of /NAMES list.\r\n");
}

void Client::quit(std::string &commandLine) {
	if (commandLine[0] == ':')
		commandLine.erase(0, 1);
	_leaveMessage = commandLine;
	send("ERROR :Closing Link: " + _IPAddress + " (Client Quit)\r\n");
	_keepAlive = false;
}

// brojola hadshi tl3li frassi hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhelpmepleasehhhhhhhhhhhhhhhhhhhhhh