
#include <iostream>

#include "logger.h"


namespace parselib::utils {

Logger::Logger(LogLevel logLevel)
	: m_log_level(logLevel)
{
}

void Logger::log_basic_info(const std::string &content) {
	log_mile(content, LogLevel::LogBasic);
}

void Logger::log_basic_err (const std::string &content) {
	log_error(content, LogLevel::LogBasic);
}

void Logger::log_all_info(const std::string &content) {
	log_mile(content, LogLevel::LogAll);
}

void Logger::log_all_err (const std::string &content) {
	log_error(content, LogLevel::LogAll);
}

void Logger::log_mile(const std::string &content, const LogLevel &level) {
	if (level <= m_log_level) {
		std::cout << "(info) " << content << std::endl ;
	}
}

void Logger::log_error(const std::string& content, const LogLevel &level) {
	if (level <= m_log_level) {
		std::cout << "(error) " << content << std::endl ;
	}
}

}
