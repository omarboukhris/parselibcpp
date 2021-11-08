
#include <iostream>

#include "logger.h"


namespace parselib {

namespace utils {

Logger::Logger(LogLevel logLevel)
	: m_log_level(logLevel)
{
}

void Logger::log_basic_info(std::string content) {
	log_mile(content, LogLevel::LogBasic);
}

void Logger::log_basic_err (std::string content) {
	log_error(content, LogLevel::LogBasic);
}

void Logger::log_all_info(std::string content) {
	log_mile(content, LogLevel::LogAll);
}

void Logger::log_all_err (std::string content) {
	log_error(content, LogLevel::LogAll);
}

void Logger::log_mile(std::string content, LogLevel level) {
	if (level <= m_log_level) {
		std::cout << "(info) " << content << std::endl ;
	}
}

void Logger::log_error(std::string content, LogLevel level) {
	if (level <= m_log_level) {
		std::cout << "(error) " << content << std::endl ;
	}
}

}

}
