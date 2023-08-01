#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <sys/socket.h>
#include <map>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <Logger.hpp>

#define PASSWORD ":1"
#define PORT 6667
		class CLogger {
			private:
				const int _id;
			public:
				CLogger(): _id(2) {}
				CLogger(int id): _id(id) {}
				void debug(std::string message) {
					::logger.debug(std::to_string(_id));
				}
				void info(std::string message) {
					::logger.info(std::to_string(_id));
				}
				void error(std::string message) {
					::logger.error(std::to_string(_id));
				}
				void warn(std::string message) {
					::logger.warn(std::to_string(_id));
				}
				void verbose(std::string message) {
					::logger.verbose(std::to_string(_id));
				}
		};
class Client {
	private:

		int _clientFd;
		std::map<std::string, void (Client::*)(std::string &)> _commands;
		std::string _nickName;
		std::string _userName;
		std::string _password;
		bool _authorized;
		static std::vector<std::string> _nickNames;
		CLogger logger;
	public:
		bool _keepAlive;
		Client(int &clientFd);
		int getFd(void);
		void send(std::string msg);
		void execute(std::string commandLine);
		void pass(std::string &commandLine);
		void nick(std::string &commandLine);
		void user(std::string &commandLine);

};


#endif