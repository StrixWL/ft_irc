#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <ctime>
#include <string>
#include <Logger.hpp>

class Logger {
	private:
		bool _debug;
		bool _verbose;
		bool _info;
		bool _error;
		bool _warn;
	public:
		Logger();
		void debug(std::string message);
		void info(std::string message);
		void verbose(std::string message);
		void warn(std::string message);
		void error(std::string message);
};

extern Logger logger;

#endif