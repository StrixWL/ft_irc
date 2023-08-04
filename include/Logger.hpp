#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <ctime>
#include <string>
#include <Logger.hpp>

// doesnt work help
#define BOLD(x) "\033[1m" x "\033[0m"

class Logger {
	private:
		bool _debug;
		bool _verbose;
		bool _info;
		bool _error;
		bool _warn;
	public:
		Logger();
		// DEBUG important messages that help detect and fix bugs
		void debug(std::string message);
		// INFO basically to log what's going in our server
		void info(std::string message);
		// VERBOSE same as DEBUG but for logs that are less important and long
		void verbose(std::string message);
		/* WARN will throw error using std::logic_error, 
			and will send whatever u give it in params back to the client
			used to warn for errors that occur during server client interaction (unexpected input, unvalid parameters, etc..) */
		void warn(std::string message);
		// ERROR for critical errors, and will write in stderr
		void error(std::string message);
};

extern Logger logger;

#endif