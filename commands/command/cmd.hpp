#pragma once

#include "../../config/config.hpp"
#include "../../file/file.hpp"
#include "../../macros/macros.hpp"
#include "../../utils/dialog.hpp"
#include "../../log/logs.hpp"

#include <charconv>
#include <filesystem>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

#ifndef TODO_VERSION
    #define TODO_VERSION "development"
#endif

namespace TODO {
    struct TodoTask {
        bool checked;
        std::string title;
    };

    class basic_command {
    protected:
        const std::vector<std::string> flags;
        log_generator logger = log_generator();

    public:
        explicit basic_command(const std::vector<std::string>& arguments)
            : flags(arguments) { }

    protected:
        static std::vector<TodoTask> read_tasks() {
            WFile file(TODO_FILE);
            const auto lines = file.read_lines();
            std::vector<TodoTask> tasks;
            tasks.reserve(lines.size());

            for (std::size_t line_index = 0; line_index < lines.size(); ++line_index) {
                const std::string& line = lines[line_index];
                if (line.empty())
                    continue;

                std::size_t position = 0;
                while (position < line.size() && line[position] >= '0' && line[position] <= '9')
                    ++position;

                unsigned int task_number = 0;
                if (position == 0 || position >= line.size() || line[position] != '.') {
                    throw todo_file_exception(
                        "Invalid Todofile entry on line " + std::to_string(line_index + 1) + "."
                    );
                }

                const auto parsed_number = std::from_chars(
                    line.data(), line.data() + position, task_number
                );
                if (parsed_number.ec != std::errc{} ||
                    task_number != tasks.size() + 1) {
                    throw todo_file_exception(
                        "Task numbers in Todofile must be consecutive, starting at 1."
                    );
                }

                ++position;
                while (position < line.size() && (line[position] == ' ' || line[position] == '\t'))
                    ++position;

                if (position + 2 >= line.size() || line[position] != '[' ||
                    (line[position + 1] != ' ' && line[position + 1] != 'X' &&
                        line[position + 1] != 'x') ||
                    line[position + 2] != ']') {
                    throw todo_file_exception(
                        "Invalid task status on Todofile line " +
                        std::to_string(line_index + 1) + "."
                    );
                }

                const bool checked = line[position + 1] == 'X' || line[position + 1] == 'x';
                position += 3;
                if (position < line.size() && line[position] == '.')
                    ++position;
                while (position < line.size() && (line[position] == ' ' || line[position] == '\t'))
                    ++position;

                tasks.push_back({checked, line.substr(position)});
            }

            return tasks;
        }

        static void write_tasks(const std::vector<TodoTask>& tasks) {
            std::string content;
            for (std::size_t index = 0; index < tasks.size(); ++index) {
                content += std::to_string(index + 1);
                content += tasks[index].checked ? ". [X] " : ". [ ] ";
                content += tasks[index].title;
                content += '\n';
            }

            WFile file(TODO_FILE);
            file.write(content.c_str());
        }

        static void validate_task_count(
            const std::vector<TodoTask>& tasks,
            std::size_t count
        ) {
            if (tasks.size() != count)
                throw todo_file_exception("Task count in count.bin does not match Todofile.");
        }

        static unsigned int parse_task_number(const std::string& text) {
            unsigned int number = 0;
            const auto result = std::from_chars(
                text.data(), text.data() + text.size(), number
            );
            if (text.empty() || result.ec != std::errc{} ||
                result.ptr != text.data() + text.size() || number == 0) {
                throw todo_file_exception("Task number must be a positive integer.");
            }
            return number;
        }

        static std::string join_arguments(
            const std::vector<std::string>& arguments,
            std::size_t first
        ) {
            std::string result;
            for (std::size_t index = first; index < arguments.size(); ++index) {
                if (!result.empty())
                    result += ' ';
                result += arguments[index];
            }
            return result;
        }

    public:
        virtual ~basic_command() = default;
        virtual bool run() = 0;
    };

    class init_command: public basic_command {
    public:
        using basic_command::basic_command;

        bool run() override {
            namespace fs = std::filesystem;
            const fs::path root(".todo");
            if (fs::exists(root)) {
                dialog::report(report_types::error, "Todofile repository already exists!");
                return false;
            }

            fs::create_directories(root / "config");
            fs::create_directory(LOG_DIRECTORY);

            WFile task_file(TODO_FILE);
            task_file.create(TODO_FILE);

            WFile counter_file(COUNTER_FILE);
            counter_file.create(COUNTER_FILE);
            counter_file.write("0");

            WFile config_file(CONFIG_FILE);
            config_file.create(CONFIG_FILE);
            config_file.write("checkable=true\nreadable=true\n");

            this->logger.generate(this->logger.log_message("Initialized Todofile repository."));

            dialog::report(report_types::success, "Todofile repository initialized.");
            return true;
        }
    };

    class list_command: public basic_command {
    public:
        using basic_command::basic_command;

        bool run() override {
            bool checked_only = false;
            bool unchecked_only = false;
            bool all = false;

            for (const auto& flag : flags) {
                if (flag == "--checked" || flag == "-c") {
                    checked_only = true;
                } else if (flag == "--unchecked" || flag == "-un") {
                    unchecked_only = true;
                } else if (flag == "-a" || flag == "--all") {
                    all = true;
                } else {
                    throw todo_file_exception("Unexpected flag: '" + flag + "'");
                }
            }

            TODOConfig config;
            const std::size_t count = config.read_count();
            const auto tasks = read_tasks();
            validate_task_count(tasks, count);
            bool printed = false;

            for (std::size_t index = 0;
                 index < count && (all || index < 20);
                 ++index) {
                if ((checked_only && !tasks[index].checked) ||
                    (unchecked_only && tasks[index].checked))
                    continue;
                std::cout << index + 1 << (tasks[index].checked ? ". [X] " : ". [ ] ")
                          << tasks[index].title << '\n';
                printed = true;
            }

            if (!printed)
                std::cout << "No tasks found.\n";
            else    
                std::printf("|- All tasks successfully shown.");

            return true;
        }
    };

    class add_command: public basic_command {
    public:
        using basic_command::basic_command;

        bool run() override {
            const std::string title = join_arguments(flags, 0);

            if (title.empty())
                throw todo_file_exception("Usage: todo add <task description>");

            TODOConfig config;
            const std::size_t count = config.read_count();
            
            auto tasks = read_tasks();
            validate_task_count(tasks, count);
            
            tasks.push_back({false, title});
            write_tasks(tasks);

            config.increment_count();

            std::string holder = std::string("Added task '");
            holder += title + "' to Todofile.";

            const char* message = logger.log_message(holder);
            this->logger.generate(message);

            dialog::report(
                report_types::success, "Added task %u.", static_cast<unsigned int>(count + 1)
            );
            return true;
        }
    };

    class remove_command: public basic_command {
    public:
        using basic_command::basic_command;

        bool run() override {
            if (flags.size() != 1)
                throw todo_file_exception("Usage: todo remove <task number>");

            const unsigned int number = parse_task_number(flags.front());
            std::string holder = std::string("Removed task '");

            TODOConfig config;
            const std::size_t count = config.read_count();

            auto tasks = read_tasks();
            validate_task_count(tasks, count);

            if (number > count)
                throw todo_file_exception("Task number does not exist.");

            holder += tasks[number - 1].title + "', of number " + std::to_string(number) + ", from Todofile.";

            tasks.erase(tasks.begin() + (number - 1));  
            write_tasks(tasks);

            config.decrement_count();

            const char* message = logger.log_message(holder);
            this->logger.generate(message);

            dialog::report(report_types::success, "Removed task %u.", number);
            return true;
        }
    };

    class help_command: public basic_command {
    public:
        using basic_command::basic_command;

        bool run() override {
            #define X(command, description, implementation) \
                std::cout << "  " << command << " - " << description << '\n';
            TODO_COMMANDS
            #undef X
            return true;
        }
    };

    class version_command: public basic_command {
    public:
        using basic_command::basic_command;

        bool run() override {
            if (!flags.empty())
                throw todo_file_exception("Usage: todo version");
            std::cout << "todo-manager " << TODO_VERSION << '\n';
            return true;
        }
    };

    class config_command: public basic_command {
    private:
        static bool parse_boolean(const std::string& value) {
            if (value == "true" || value == "1" || value == "on")
                return true;
            if (value == "false" || value == "0" || value == "off")
                return false;
            throw todo_file_exception("Expected true/false (or on/off) for config value.");
        }

    public:
        using basic_command::basic_command;

        bool run() override {
            if (flags.empty()) {
                WFile file(CONFIG_FILE);
                std::cout << file.read();
                return true;
            }
            if (flags.size() != 2)
                throw todo_file_exception("Usage: todo config <checkable|readable> <true|false>");

            if (flags[0] != "checkable" && flags[0] != "readable")
                throw todo_file_exception("Config option must be 'checkable' or 'readable'.");

            WFile file(CONFIG_FILE);
            std::string content = file.read();
            std::string copy    = content;

            const std::string setting = flags[0] + "=" +
                (parse_boolean(flags[1]) ? "true" : "false");

            const auto setting_position = content.find(flags[0] + "=");
            if (setting_position == std::string::npos) {
                if (!content.empty() && content.back() != '\n')
                    content += '\n';
                content += setting + '\n';
            } else {
                const auto line_end = content.find('\n', setting_position);
                content.replace(
                    setting_position,
                    line_end == std::string::npos
                        ? content.size() - setting_position
                        : line_end - setting_position,
                    setting
                );
            }

            std::string holder = std::string("Configuration '");
            holder += flags[0] + "' was set to '" + flags[1] + "'.";

            auto message = logger.log_message(holder);
            logger.generate(message);

            file.write(content.c_str());
            dialog::report(report_types::success, "%s set to %s.", flags[0].c_str(), flags[1].c_str());
            return true;
        }
    };

    class switch_command: public basic_command {
    public:
        using basic_command::basic_command;

        bool run() override {
            if (flags.size() != 1)
                throw todo_file_exception("Usage: todo switch <task number>");

            const unsigned int number = parse_task_number(flags.front());
            
            TODOConfig config;
            const std::size_t count = config.read_count();

            auto tasks = read_tasks();
            validate_task_count(tasks, count);

            if (number > count)
                throw todo_file_exception("Task number does not exist.");

            tasks[number - 1].checked = !tasks[number - 1].checked;

            std::string holder = std::string(((tasks[number -1].checked) ? "Checked" : "Unchecked"));
            holder += " task '" + tasks[number - 1].title + "', of number " + std::to_string(number) + ".";
            
            write_tasks(tasks);

            auto message = logger.log_message(holder);
            logger.generate(message);
            
            dialog::report(report_types::success, "Task %u is now %s.", number, tasks[number - 1].checked ? "checked" : "unchecked");
            return true;
        }
    };

    class count_command: public basic_command {
    public:
        using basic_command::basic_command;

        bool run() override {
            if (flags.size() > 1)
                throw todo_file_exception("Usage: todo count [--checked|--unchecked]");

            TODOConfig config;
            std::size_t count = config.read_count();
            if (!flags.empty()) {
                if (flags.front() == "--checked" || flags.front() == "-c") {
                    const auto tasks = read_tasks();
                    validate_task_count(tasks, count);
                    count = 0;
                    for (const auto& task : tasks)
                        count += task.checked ? 1 : 0;
                } else if (flags.front() == "--unchecked" || flags.front() == "-un") {
                    const auto tasks = read_tasks();
                    validate_task_count(tasks, count);
                    count = 0;
                    for (const auto& task : tasks)
                        count += task.checked ? 0 : 1;
                } else {
                    throw todo_file_exception("Unexpected flag: '" + flags.front() + "'");
                }
            }

            TODO::dialog::report(
                TODO::report_types::log, "Actual count: %llu",
                static_cast<unsigned long long>(count)
            );
            return true;
        }
    };

    class edit_command: public basic_command {
    public:
        using basic_command::basic_command;

        bool run() override {
            if (flags.empty()) {
                dialog::report(report_types::error, "no argument was given.");
                return false;
            }

            if (flags.size() <= 1) {
                dialog::report(report_types::error, "too few arguments.");
                return false;
            }

            if (flags.size() != 2) {
                dialog::report(report_types::error, "too many arguments!");
                return false;
            }

            WFile file = WFile(TODO_FILE);

            int index = std::stoi(flags[0]);

            if (index <= 0) {
                dialog::report(report_types::error, "wrong index was given: %d", index);
                return false;
            }

            const std::string previous_content = file.read(index);
            std::string content = previous_content;

            const std::size_t status_end = content.find(']');
            if (status_end == std::string::npos)
                throw todo_file_exception("Task status is missing from the selected line.");

            std::size_t title_start = status_end + 1;
            if (title_start < content.size() && content[title_start] == '.')
                ++title_start;
            while (title_start < content.size() &&
                (content[title_start] == ' ' || content[title_start] == '\t')) {
                ++title_start;
            }

            std::string copy = content;
            content.replace(title_start, content.size() - title_start, flags[1]);

            std::string holder = std::string("Edited task of number ");
            holder += std::to_string(index) + ", from '" + copy + "' to '" + content + "'.";

            auto message = logger.log_message(holder);
            logger.generate(message);

            file.print(content.c_str(), index);

            dialog::report(report_types::info,
                "Task of number %d was edited.\n",
                index, previous_content.c_str(), content.c_str());

            return true;
        }
    };

    class check_command: public basic_command {
    public:
        using basic_command::basic_command;

        bool run() override {
            if (!flags.empty())
                throw todo_file_exception("Usage: todo check");

            TODOConfig config;
            const std::size_t count = config.read_count();
            const auto tasks = read_tasks();
            validate_task_count(tasks, count);

            auto message = logger.log_message("Validated Todofile repository.");
            logger.generate(message);

            dialog::report(report_types::success, 
                "Todofile repository is valid (%llu tasks).", static_cast<unsigned long long>(count));
            return true;
        }
    };
}
