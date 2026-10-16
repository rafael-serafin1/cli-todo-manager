#pragma once

#ifndef DATE_TIME_H
#define DATE_TIME_H

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <string>


class date_time {
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
    date_time(): log_date(std::time(nullptr)) {
        log_local = std::localtime(&log_date);
        
        year     =  log_local->tm_year + 1900;
        month    =  log_local->tm_mon  + 1;
        day      =  log_local->tm_mday;
 
        hour     =  log_local->tm_hour;
        minute   =  log_local->tm_min;
        seconds  =  log_local->tm_sec;
    }

    int get_day() {
        return day;
    }

    int get_month() {
        return month;
    }

    int get_year() {
        return year;
    }

    int get_hour() {
        return hour;
    }

    int get_minute() {
        return minute;
    }

    int get_seconds() {
        return seconds;
    }

    std::tm* get_time() {
        return log_local;
    }


    const char* date() {
        char* buffer = new char[2048];
        
        std::sprintf(buffer, "%d-%d-%d", month, day, year);

        return buffer;
    }

    const char* time() {
        char* buffer = new char[2048];
        
        std::sprintf(buffer, "%d:%d:%d", hour, minute, seconds);

        return buffer;
    }

    const char* now() {
        char* buffer = new char[2048];
        
        std::sprintf(buffer, "%d-%d-%d %d:%d:%d", year, day, month, hour, minute, seconds);

        return buffer;
    }
};

#endif