#pragma once
#include <exception>
#include <string>

namespace TODO {
    enum class TodoStatus: bool {
        SUCCESS = 0,
        FAILURE = 1
    };

    class todo_file_exception: public std::exception {
    protected:
        std::string _message;
    public:
        explicit todo_file_exception(const std::string& message): _message(message) { }

        const char* what() const noexcept override {
            return _message.c_str();
        }
    };
};