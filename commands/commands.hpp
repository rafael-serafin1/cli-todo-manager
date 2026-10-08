#include "../file/file.hpp"
#include "../parse/parse.hpp"

#ifndef COMMANDS_H
#define COMMANDS_H
using namespace TODO;

namespace TODO {
    class TODOCommand {
    private:
        TODOParser::__TODO_CLI__ cli;

    public:
        TODOCommand(TODOParser::__TODO_CLI__ _Struct): cli(_Struct) { }

        void execute();
    };
};

#endif