#include <Logger.hpp>

Logger::Logger() {
	_debug = true;
	_warn = true;
	_info = true;
	_error = true;
	_verbose = true;

	/* Uncomment to mute */
	// _debug = false;
	// _warn = false;
	// _info = false;
	// _error = false;
	// _verbose = false;
}

std::string getTime() {
	// le time
	std::time_t now = std::time(NULL);
    std::tm *timeinfo = std::localtime(&now);
	std::string hour = std::to_string(timeinfo->tm_hour);
	hour = hour.length() == 2 ? hour : "0" + hour;
	std::string minute = std::to_string(timeinfo->tm_min);
	minute = minute.length() == 2 ? minute : "0" + minute;
	std::string second = std::to_string(timeinfo->tm_sec);
	second = second.length() == 2 ? second : "0" + second;
	return ((hour.length() == 2 ? hour : "0" + hour) + ":" + minute + ":" + second);
	// return "-";
}

void Logger::debug(std::string message) {
	if (!_debug)
		return ;
	std::cout << "\033[0;90m" << "[" << getTime() << "]" << "\033[1;36m";
	std::cout << " DEBUG " <<  "\033[0;97m";
	std::cout << message << "\033[0m" << std::endl;
}

void Logger::info(std::string message) {
	if (!_info)
		return ;
	std::cout << "\033[0;90m" << "[" << getTime() << "]" << "\033[1;34m";
	std::cout << " INFO " <<  "\033[0;97m";
	std::cout << message << "\033[0m" << std::endl;
}

void Logger::error(std::string message) {
	if (!_error)
		return ;
	std::cerr << "\033[0;90m" << "[" << getTime() << "]" << "\033[1;31m";
	std::cerr << " ERROR " <<  "\033[0;97m";
	std::cerr << message << "\033[0m" << std::endl;
}

void Logger::verbose(std::string message) {
	if (!_verbose)
		return ;
	std::cout << "\033[0;90m" << "[" << getTime() << "]" << "\033[1;32m";
	std::cout << " VERBOSE " <<  "\033[0;97m";
	std::cout << message << "\033[0m" << std::endl;
}

void Logger::warn(std::string message) {
	if (!_warn)
		return ;
	std::cout << "\033[0;90m" << "[" << getTime() << "]" << "\033[1;33m";
	std::cout << " WARN " <<  "\033[0;97m";
	std::cout << message << "\033[0m" << std::endl;
}

// declaring logger, this is an extern variable
Logger logger;
