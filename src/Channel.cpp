#include "Channel.hpp"

Channel::Channel(std::string &name): _name(name) {
}

// broadcast to all members
void Channel::broadcast(std::string message) {
	for (std::vector<Client *>::iterator it = _members.begin(); it != _members.end(); it++)
		(*it)->send(message);
}

// broadcast to all members except the broadcaster
void Channel::broadcast(std::string message, Client *broadcaster) {
	for (std::vector<Client *>::iterator it = _members.begin(); it != _members.end(); it++)
		if (*it != broadcaster)
			(*it)->send(message);
}

// _members.map(e => _operators.includes(e) ? "@" + e : e).join(" ");
std::string Channel::getNames(void) {
	std::string names = "";
	for (std::vector<Client *>::iterator it = _members.begin(); it != _members.end(); it++)
		names += std::string(names.length() ? " " : "") + ((std::find(_operators.begin(), _operators.end(), *it) !=  _operators.end()) ? "@" : "") + (*it)->_nickName; // XD?
	return names;
}
