
#include <iostream>

#include <parselib/utils/logger.h>


namespace parselib::utils {


void ConsoleLogger::info(const std::string &content) const  {
    auto t = std::chrono::system_clock::now();
    auto ss_time = std::chrono::system_clock::to_time_t(t);
    std::cout << ss_time << component << " : " << content << std::endl;
}

void ConsoleLogger::warn(const std::string &content) const  {
    auto t = std::chrono::system_clock::now();
    auto ss_time = std::chrono::system_clock::to_time_t(t);
    std::cout << ss_time << component << content << std::endl;
}

void ConsoleLogger::err(const std::string &content) const {
    auto t = std::chrono::system_clock::now();
    auto ss_time = std::chrono::system_clock::to_time_t(t);
    std::cerr << ss_time << component << content << std::endl;
}

}
