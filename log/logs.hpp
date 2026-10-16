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
public:
    log_generator() { }

    ~log_generator() { }

    const char* log_message(const std::string& Additional) {
        date_time dt;

        std::string log = std::string(dt.now());
        log = log + " " + Additional;

        const char* Log = log.c_str();
        return Log;
    }

    void generate(const char* _Message) {
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

        try {
            f.create(path.c_str());
        } 
        catch (const TODO::todo_file_exception& ex) { 
            TODO::dialog::report(report_types::warn, "File '%s' already exists.", path.c_str());
        }

        f.append(_Message);
    }
};

#endif 