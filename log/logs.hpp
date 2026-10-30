#pragma once

#ifndef LOGS_H
#define LOGS_H

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <string>

#include "../utils/dialog.hpp"
#include "../file/file.hpp"
#include "../macros/macros.hpp"
#include "../utils/date_time.hpp"

class log_generator {
private:
    void create_log_file(const std::string& path) {
        TODO::WFile f = TODO::WFile(path.c_str());

        try {
            f.create(path.c_str());
        } 
        catch (const TODO::todo_file_exception& ex) { 
            TODO::dialog::report(report_types::warn, "File '%s' already exists.", path.c_str());
        }
    }
    
public:
    log_generator() { }

    ~log_generator() { }

    std::string log_message(const std::string& Additional) {
        date_time dt;

        std::string log = std::string(dt.now());
        log = log + " " + Additional;

        return log;
    }

    void generate(const std::string& message) {
    #ifdef _WIN32
        std::string path = std::string(LOG_DIRECTORY);
        path += '\\';
    #else
        std::string path = std::string(LOG_DIRECTORY);
        path += '/';
    #endif
        date_time dt;

        std::ostringstream stream;
        auto log_local = dt.get_time();

        stream << std::put_time(log_local, "%m-%d-%y");

        path += stream.str() + ".log";
        
        TODO::WFile f = TODO::WFile(path.c_str());
        create_log_file(path);

        f.append(message.c_str());
    }
};

#endif 