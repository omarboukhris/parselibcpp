#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <memory>

namespace parselib {

namespace utils {

class Logger
{
public:

	enum class LogLevel {
		LogNone,
		LogBasic,
		LogAll
	};

	Logger(LogLevel logLevel=LogLevel::LogNone);

	void log_basic_info(const std::string &content) ;
	void log_all_info (const std::string &content) ;

	void log_basic_err (const std::string &content) ;
	void log_all_err  (const std::string &content) ;

private:

	inline void log_mile(const std::string &content, const LogLevel &level);

	inline void log_error(const std::string& content, const LogLevel &level);

private:
	LogLevel m_log_level;
};

using LogLevel = Logger::LogLevel;
using LoggerPtr = std::shared_ptr<Logger> ;

}

}

#endif // LOGGER_H
