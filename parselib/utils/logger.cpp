
#include <iostream>

#include "logger.h"


namespace parselib {

namespace utils {

Logger::Logger(int logLevel)
	: m_log_level(logLevel)
{
}


void Logger::log_basic_info(std::string content) {
	log_mile(content, LogBasic);
}

void Logger::log_basic_err (std::string content) {
	log_error(content, LogBasic);
}

void Logger::log_all_info(std::string content) {
	log_mile(content, LogAll);
}

void Logger::log_all_err (std::string content) {
	log_error(content, LogAll);
}


void Logger::log_mile(std::string content, int level) {
	if (level <= m_log_level) {
		std::cout << "(info) " << content << std::endl ;
	}
}

void Logger::log_error(std::string content, int level) {
	if (level <= m_log_level) {
		std::cout << "(error) " << content << std::endl ;
	}
}

}

}
