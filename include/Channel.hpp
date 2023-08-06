#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "Client.hpp"

class Channel {
	public:
		Channel(std::string &name);
		std::string _name;
		std::vector<Client *> _members;
		// broadcast to all members
		void broadcast(std::string message);
		// broadcast to all members except the broadcaster
		void broadcast(std::string message, Client *broadcaster);
};

#endif
