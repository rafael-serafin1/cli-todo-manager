#include "commands.hpp"
#include "command/cmd.hpp"

#include <memory>

namespace TODO {
    void TODO::TODOCommand::execute() {
        std::unique_ptr<basic_command> command;

        #define X(command_name, description, implementation) \
            if (this->cli.command == command_name) \
                command = std::make_unique<implementation>(this->cli.flags);
            TODO_COMMANDS
        #undef X

        if (command == nullptr)
            throw TODO::todo_file_exception("Unknown command: '" + this->cli.command + "'");

        if (!command->run())
            throw TODO::todo_file_exception("Error during command execution.");
    }
}