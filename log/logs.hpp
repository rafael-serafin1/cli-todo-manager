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

class log_generator {
private:
    std::time_t log_date;
    std::tm*    log_local;

    int year;
    int month;
    int day;
    
    int hour;
    int minute;
    int seconds;

public:
    log_generator():
        log_date(std::time(nullptr))
    {
        log_local = std::localtime(&log_date);
        
        year  = log_local->tm_year + 1900;
        month = log_local->tm_mon  + 1;
        day   = log_local->tm_mday;

        hour    = log_local->tm_hour;
        minute  = log_local->tm_min;
        seconds = log_local->tm_sec;
    }

    ~log_generator() { }

    const char* log_message(const std::string& Additional) {
        char* buffer = new char[2048];
        
        std::sprintf(buffer, "%d-%d-%d %d:%d:%d %s", year, day, month, hour, minute, seconds, Additional.c_str());

        return buffer;
    }

    void generate(const char* _Message) {
    #ifdef _WIN32
        std::string path = std::string(LOG_DIRECTORY);
        path += '\\';
    #else
        std::string path = std::string(LOG_DIRECTORY);
        path += '/';
    #endif
        std::ostringstream stream;
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