#include <iostream>
#include "parse.hpp"

namespace TODO {
    TODO::TODOParser::__TODO_CLI__ TODOParser::parse_all() {
        return __TODO_CLI__{
            parse_command(),
            parse_flags()
        };
    }

    std::string TODO::TODOParser::parse_command() {
        if (n_args < 2)
            throw TODO::todo_file_exception("Esperado +2 argumentos...");

        return std::string(argv[1]);
    }

    std::vector<std::string> TODO::TODOParser::parse_flags() {
        std::vector<std::string> vetor;

        for (int i = 2; i < n_args; ++i)
            vetor.push_back(argv[i]);

        return vetor;
    }
};