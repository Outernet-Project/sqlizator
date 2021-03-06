// Copyright 2015, Outernet Inc.
// Some rights reserved.
// This software is free software licensed under the terms of GPLv3. See COPYING
// file that comes with the source code, or http://www.gnu.org/licenses/gpl.txt.
#ifndef SQLIZATOR_SQLIZATOR_DATABASE_H_
#define SQLIZATOR_SQLIZATOR_DATABASE_H_
#include <sqlite3.h>
#include <msgpack.hpp>

#include <string>
#include <vector>

#include "sqlizator/response.h"

namespace sqlizator {

enum Operation {
    EXECUTE = 1,
    EXECUTE_AND_FETCH = 2
};

const std::vector<std::string> PRAGMAS{"journal_mode", "foreign_keys"};

class Database {
 private:
    sqlite3* db_;
    std::string path_;
 public:
    explicit Database(const std::string& path);
    ~Database();
    void connect();
    void close();
    void pragma(const std::string& key, const std::string& value);
    void query(Operation operation,
               const std::string& query,
               const msgpack::object_handle& parameters,
               Packer* header,
               Packer* data);
    std::string path();
};

}  // namespace sqlizator
#endif  // SQLIZATOR_SQLIZATOR_DATABASE_H_
