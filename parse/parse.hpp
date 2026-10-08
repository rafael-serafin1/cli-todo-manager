#pragma once

#include <iostream>
#include <vector>

#include "../index.hpp"

#ifndef PARSE_H
#define PARSE_H

namespace TODO {
    class TODOParser {
    private:
        int n_args = 0;
        char** argv = nullptr;

    public:
        TODOParser(int argc, char** argv) 
            : n_args(argc), argv(argv) { }
        
        typedef struct {
            const std::string command;
            const std::vector<std::string> flags;
        } __TODO_CLI__;

        __TODO_CLI__ parse_all();

        std::string parse_command();

        std::vector<std::string> parse_flags();
    };
};

#endif