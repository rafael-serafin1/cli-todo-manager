#include <string>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>

std::string trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();

    while (start < end && std::isspace(static_cast<unsigned char>(str[start])))     ++start;
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1])))   --end;

    return str.substr(start, end - start);
}

std::string toupper(const std::string& str) {
    std::string result;

    for (std::size_t i = 0; i < str.size(); ++i)
        result += toupper(str[i]);

    return result;
}

std::string get_date() {
    date_time dt;

    std::ostringstream stream;
    auto log_local = dt.get_time();

    stream << std::put_time(log_local, "%m-%d-%y");
    return stream.str();
}

void capitalize(std::string* str) {
    if (str->size() <= 0)
        return;

    (*str)[0] = toupper((*str)[0]);
}

// TODO: trim all whitespaces in a string