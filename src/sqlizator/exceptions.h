// Copyright 2015, Outernet Inc.
// Some rights reserved.
// This software is free software licensed under the terms of GPLv3. See COPYING
// file that comes with the source code, or http://www.gnu.org/licenses/gpl.txt.
#ifndef SQLIZATOR_SQLIZATOR_EXCEPTIONS_H_
#define SQLIZATOR_SQLIZATOR_EXCEPTIONS_H_
#include <stdexcept>
#include <string>

namespace sqlizator {

class sqlite_error: public std::runtime_error {
 private:
    std::string extended_;
 public:
    explicit sqlite_error(const std::string& message):
                                                std::runtime_error(message) {}
    explicit sqlite_error(const std::string& message,
                          const std::string& extended):
                                                std::runtime_error(message),
                                                extended_(extended) {}
    const std::string extended() {
        return extended_;
    }
};

class invalid_request: public std::runtime_error {
 public:
    explicit invalid_request(const std::string& message):
                                                std::runtime_error(message) {}
};

class message_error: public std::runtime_error {
 public:
    explicit message_error(const std::string& message):
                                                std::runtime_error(message) {}
};

}  // namespace sqlizator
#endif  // SQLIZATOR_SQLIZATOR_EXCEPTIONS_H_
