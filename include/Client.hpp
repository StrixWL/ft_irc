#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <sys/socket.h>
#include <map>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <Logger.hpp>

#define PASSWORD "123"

class Client {
	// bai bai old logger, each client will have its own logger now
	/* this will be used to construct a logger for each client so that we could output
			the client id before each log message, cool right? */
	class Logger {
		public:
			const int _id;
			Logger(int id): _id(id) {}
			void debug(std::string message) {
				::logger.debug("\033[1;37m[" + std::to_string(_id) + "]\033[0m " + message);
			}
			void info(std::string message) {
				::logger.info("\033[1;37m[" + std::to_string(_id) + "]\033[0m " +  message);
			}
			void error(std::string message) {
				::logger.error("\033[1;37m[" + std::to_string(_id) + "]\033[0m " +  message);
			}
			// using warn to throw errors back to the client
			void warn(std::string message)  {
				::logger.warn("\033[1;37m[" + std::to_string(_id) + "]\033[0m " +  message);
				throw std::logic_error(message + "\r\n");
			}
			void verbose(std::string message) {
				::logger.verbose("\033[1;37m[" + std::to_string(_id) + "]\033[0m " +  message);
			}
	};
	public: // private
		int _clientFd;
		std::map<std::string, void (Client::*)(std::string &)> _commands;
		std::string _nickName;
		std::string _userName;
		std::string _realName;
		std::string _IPAddress;
		std::string _password;
		std::string _leaveMessage;
		// authorized means client entered correct password (only)
		bool _authorized;
		// registered means client entered correct password, a valid nickname, username and realname 
		bool _registered;
		// welcomed means this used was registered before and welcomed, used so that we dont send welcome messages everytime user changes nick
		bool _welcomed;
		Client::Logger logger;
	public:
		bool _keepAlive;
		Client(int &clientFd);
		int getFd(void);
		void send(std::string msg);
		void execute(std::string commandLine);
		void welcome(void);
		void pass(std::string &commandLine);
		void nick(std::string &commandLine);
		void user(std::string &commandLine);
		void join(std::string &commandLine);
		void privmsg(std::string &commandLine);
		void part(std::string &commandLine);
		void kick(std::string &commandLine);
		void mode(std::string &commandLine);
		void invite(std::string &commandLine);
		void topic(std::string &commandLine);
		void quit(std::string &commandLine);
};


#endif