#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <memory>

namespace parselib {

namespace utils {

class Logger
{
public:

	enum {
		LogNone=0,
		LogBasic=1,
		LogAll=2
	};

	Logger(int logLevel=LogNone);

	void log_basic_info(std::string content) ;
	void log_all_info (std::string content) ;

	void log_basic_err (std::string content) ;
	void log_all_err  (std::string content) ;

private:

	inline void log_mile(std::string content, int level);

	inline void log_error(std::string content, int level);

private:
	int m_log_level;
};

typedef std::shared_ptr<Logger> Logger_ptr;

}

}

#endif // LOGGER_H
