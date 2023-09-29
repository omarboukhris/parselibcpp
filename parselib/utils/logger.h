#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <utility>

namespace parselib::utils {

class ConsoleLogger
{
public:

    ConsoleLogger() = default;
    explicit ConsoleLogger(std::string c) : component(std::move(c)) {}

    void info(const std::string &content) const;

    void warn(const std::string &content) const;

    void err (const std::string &content) const ;

    std::string component;
};

using LoggerPtr = std::shared_ptr<ConsoleLogger> ;

}
