#include "Channel.hpp"

Channel::Channel(std::string &name): _name(name) {
}

void Channel::broadcast(std::string message) {
	for (std::vector<Client *>::iterator it = _members.begin(); it != _members.end(); it++) {
		(*it)->send(message);
	}
}

void Channel::broadcast(std::string message, Client *broadcaster) {
	for (std::vector<Client *>::iterator it = _members.begin(); it != _members.end(); it++) {
		if (*it != broadcaster)
			(*it)->send(message);
	}
}