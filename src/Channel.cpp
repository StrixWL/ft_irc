#include "Channel.hpp"

Channel::Channel(std::string &name): _name(name) {
}

// broadcast to all members
void Channel::broadcast(std::string message) {
	for (std::vector<Client *>::iterator it = _members.begin(); it != _members.end(); it++) {
		(*it)->send(message);
	}
}

// broadcast to all members except the broadcaster
void Channel::broadcast(std::string message, Client *broadcaster) {
	for (std::vector<Client *>::iterator it = _members.begin(); it != _members.end(); it++) {
		if (*it != broadcaster)
			(*it)->send(message);
	}
}
