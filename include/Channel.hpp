#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "Client.hpp"

class Channel {
	public:
		Channel(std::string &name);
		std::string _name;
		std::string _password;
		std::vector<Client *> _members;
		std::vector<Client *> _operators;
		// broadcast to all members
		void broadcast(std::string message);
		// broadcast to all members except the broadcaster
		void broadcast(std::string message, Client *broadcaster);
		// broadcast to operators except the broadcaster
		void broadcastOP(std::string message, Client *broadcaster);
		std::string getNames(void);
};

#endif
