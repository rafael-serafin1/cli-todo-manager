#include <iostream>

#include "index.hpp"
#include "utils/dialog.hpp"

#include "parse/parse.hpp"
#include "commands/commands.hpp"

using namespace TODO;

int main(int argc, char** argv) {
    TodoStatus status = TodoStatus::SUCCESS;

    try {
        if (argc < 2) {
            status = TodoStatus::FAILURE;
            throw todo_file_exception("--> Expected 2+ arguments.");
        }

        TODOParser parser = TODOParser(argc, argv);
        auto parsed = parser.parse_all();

        TODOCommand caller = TODOCommand(parsed);

        caller.execute();
    } catch (const todo_file_exception& ex) {
        dialog::report(report_types::error, "%s", ex.what());
        status = TodoStatus::FAILURE;
    } catch (const std::exception& ex) {
        dialog::report(report_types::error, "%s", ex.what());
        status = TodoStatus::FAILURE;
    }

    return (bool) status;
}