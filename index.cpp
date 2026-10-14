#include <iostream>
#include <string>

#ifdef _WIN32
    #include <windows.h>
#endif

#include "index.hpp"
#include "utils/dialog.hpp"

#include "parse/parse.hpp"
#include "commands/commands.hpp"

using namespace TODO;


#ifdef _WIN32

std::string utf16_to_utf8(const wchar_t* input) {
    if (!input || !*input)
        return {};

    const int size = WideCharToMultiByte(
        CP_UTF8, WC_ERR_INVALID_CHARS, input, -1, nullptr, 0, nullptr, nullptr
    );

    if (size == 0)
        throw todo_file_exception("Failed to convert command-line argument to UTF-8.");

    std::string output(size, '\0');

    if (WideCharToMultiByte(
            CP_UTF8, WC_ERR_INVALID_CHARS, input, -1,
            output.data(), size, nullptr, nullptr
        ) == 0) {
        throw todo_file_exception("Failed to convert command-line argument to UTF-8.");
    }

    output.resize(size - 1);
    return output;
}


int wmain(int argc, wchar_t* wargv[]) {
    SetConsoleOutputCP(CP_UTF8);

    TodoStatus status = TodoStatus::SUCCESS;

    try {
        if (argc < 2) {
            status = TodoStatus::FAILURE;
            throw todo_file_exception("--> Expected 2+ arguments.");
        }

        std::vector<std::string> args;
        args.reserve(argc);

        for (int i = 0; i < argc; ++i)
            args.push_back(utf16_to_utf8(wargv[i]));

        std::vector<char*> argv;
        argv.reserve(argc);

        for (auto& arg : args)
            argv.push_back(arg.data());

        TODOParser parser = TODOParser(argc, argv.data());
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

#else

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

#endif