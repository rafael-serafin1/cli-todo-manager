#pragma once

#include "../../config/config.hpp"
#include "../../file/file.hpp"
#include "../../macros/macros.hpp"
#include "../../utils/dialog.hpp"
#include "../../log/logs.hpp"
#include "../../link/link.hpp"

#include "subtasks/subtasks.hpp"
#include "subtasks/subtask_sql.hpp"
#include "subtasks/subtask_nosql.hpp"

#include <charconv>
#include <filesystem>
#include <iostream>
#include <limits>
#include <string>
#include <system_error>
#include <vector>
#include <format> 


#include <thread>
#include <future>
#include <chrono>

#ifndef TODO_VERSION
    #define TODO_VERSION "development"
#endif

#define BASIC_CONFIG "checkable=true\nreadable=true\neditable=true\nnosql=false\n"

namespace fs = std::filesystem;

namespace TODO {
    struct TodoTask {
        bool checked;
        std::string title;
    };

    class basic_command {
    protected:
        const std::vector<std::string> flags;
        log_generator logger = log_generator();

        bool has_flag(const std::vector<std::string>& flags, const std::string& flag, const std::string& alias) {
            for (const std::string& i: flags)
                if (i == flag or i == alias)
                    return true;

            return false;
        }   

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

                if (position == 0 || position >= line.size() || line[position] != '.')
                    throw todo_file_exception("Invalid Todofile entry on line " + std::to_string(line_index + 1) + ".");

                const auto parsed_number = std::from_chars(line.data(), line.data() + position, task_number);

                if (parsed_number.ec != std::errc{} || task_number != tasks.size() + 1)
                    throw todo_file_exception("Task numbers in Todofile must be consecutive, starting at 1.");

                ++position;
                while (position < line.size() && (line[position] == ' ' || line[position] == '\t'))
                    ++position;

                if (position + 2 >= line.size() || line[position] != '[' ||
                    (line[position + 1] != ' ' && line[position + 1] != 'X' &&
                        line[position + 1] != 'x') ||
                    line[position + 2] != ']') {
                    throw todo_file_exception("Invalid task status on Todofile line " + std::to_string(line_index + 1) + ".");
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

        static void validate_task_count(const std::vector<TodoTask>& tasks, std::size_t count) {
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

        static std::string join_arguments(const std::vector<std::string>& arguments, std::size_t first) {
            std::string result;

            for (std::size_t index = first; index < arguments.size(); ++index) {
                if (!result.empty())
                    result += ' ';

                if (!arguments[index].starts_with("--") && !arguments[index].starts_with("-"))
                    result += arguments[index];
            }

            return result;
        }

        static std::vector<std::string> match_flags(const std::string& Cmd, const std::vector<std::string>& flags) {
            std::vector<std::string> resultado;

            for (const auto& i: flags) {
                #define X(cmd, flag, alias, desc)     \
                    if (cmd == Cmd && (i == flag or i == alias)) { resultado.push_back(i); continue; }
                    TODO_FLAGS
                #undef X
            }

            return resultado;
        }

        static bool validate_if_task_exists(const std::vector<TodoTask>& tasks, std::size_t id) {
            for (std::size_t i = 0; i < tasks.size(); ++i)
                if (i == id)
                    return true;

            return false;
        }

    public:
        virtual ~basic_command() = default;
        virtual bool run() = 0;
    };

    class init_command: public basic_command {
    private:
        bool normal_init() {
            namespace fs = std::filesystem;
            const fs::path root(TODO_REPO);
            
            if (fs::exists(root)) {
                dialog::report(report_types::error, "Todofile repository already exists!");
                return false;
            }

            fs::create_directories(root / "config");

        #ifdef _WIN32
            if (!SetFileAttributesA(TODO_REPO, FILE_ATTRIBUTE_HIDDEN)) {
                dialog::report(report_types::error, "Failed to hide directory.");
                return false;
            }
        #endif

            fs::create_directory(LOG_DIRECTORY);

            WFile task_file(TODO_FILE);
            task_file.create(TODO_FILE);

            WFile counter_file(COUNTER_FILE);
            counter_file.create(COUNTER_FILE);
            counter_file.write("0");

            WFile config_file(CONFIG_FILE);
            config_file.create(CONFIG_FILE);

            WFile link_file(LINK_FILE);
            link_file.create();

            WFile link_count(LINK_COUNT);
            link_count.create();

            bool nosql = false;
            if (flags.size() > 0) 
                if (has_flag(flags, "--nosql", "-ns"))
                    nosql = true;

            const std::string CONFIG = std::string(std::format("checkable=true\nreadable=true\nnosql={}\n", ((nosql) ? "true" : "false")));

            config_file.write(CONFIG.c_str());

            this->logger.generate(this->logger.log_message("Initialized Todofile repository."));

            dialog::report(report_types::success, "Todofile repository initialized.");

            return true;
        }

        void subtask_init() {
            TODOConfig config;
            const auto& c = config.read_config();

            if (c.nosql) {
                if (fs::exists(TODO_SUBTASKS_NOSQL))
                    throw todo_file_exception("noSQL-based database already exists.");

                auto handler = subtask_nosql_handler();
                handler.create_database();
                handler.create_table();
            } else {
                if (fs::exists(TODO_SUBTASKS_SQL)) 
                    throw todo_file_exception("SQL-based database already exists.");

                auto handler = subtask_sql_handler();
                handler.create_database();
                handler.create_table();
            }
        }

    public:
        using basic_command::basic_command;

        bool run() override {
            namespace fs = std::filesystem;

            if (has_flag(flags, "--subtasks", "-st")) {
                if (!fs::exists(TODO_REPO)) {
                    dialog::report(report_types::warn, "No Todofile repo was found.");
                    return false;
                }

                this->subtask_init();
                return true;
            } 
            else if (flags.size() > 0)
                throw todo_file_exception("Too much arguments.");
            
            return this->normal_init();
        }
    };

    class list_command: public basic_command {
    public:
        using basic_command::basic_command;

        bool run() override {
            bool printed = false;

            bool checked_only = false;
            bool unchecked_only = false;
            bool all = false;

            for (const auto& flag : flags) {
                if (flag == "--checked" || flag == "-c") {
                    checked_only = true;
                } 
                else if (flag == "--unchecked" || flag == "-un") {
                    unchecked_only = true;
                } 
                else if (flag == "-a" || flag == "--all") {
                    all = true;
                } 
                else if (flag == "-l" || flag == "--logs") {
                    for (const auto& i: fs::directory_iterator(LOG_DIRECTORY))
                        if (i.is_regular_file() && i.path().extension() == ".log")
                            dialog::report(report_types::info, "%s", i.path().string().c_str());

                    return true;
                }
                else {
                    throw todo_file_exception("Unexpected flag: '" + flag + "'");
                }
            }

            TODOConfig config;
            const std::size_t count = config.read_count();
            
            const auto tasks = read_tasks();
            validate_task_count(tasks, count);

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
                dialog::report(report_types::warn, "No tasks found.");
            else    
                dialog::report(report_types::info, "All tasks successfully shown.");

            return true;
        }
    };

    class add_command: public basic_command {
    private:
        bool has_flag(const std::string& flag) {
            for (const std::string& i: this->flags)
                if (i == flag)
                    return true;

            return false;
        }   

        bool has_flag(const std::string& flag, const std::string& alias) {
            for (const std::string& i: this->flags)
                if (i == flag || i == alias)
                    return true;

            return false;
        }   

    public:
        using basic_command::basic_command;

        bool run() override {
            const std::string title = join_arguments(flags, 0);

            if (title.empty())
                throw todo_file_exception("Usage: todo add <task description>");

            TODOConfig config;
            const std::size_t count = config.read_count();

            auto f = match_flags("add", flags);
            const bool check = basic_command::has_flag(f, "--done", "-d");
            
            auto tasks = read_tasks();
            validate_task_count(tasks, count);
            
            tasks.push_back({check, title});
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
    private:
        void show_all_commands() {
            #define X(command, description, implementation) \
                std::cout << "  " << command << " - " << description << '\n';
            TODO_COMMANDS
            #undef X
        }

        void specify_command(const std::string& command) {
            bool found = false;

            #define X(command_name, description, implementation) \
                if (command == command_name) { \
                    std::cout << command_name << " - " << description << '\n'; \
                    found = true; \
                }
            TODO_COMMANDS
            #undef X

            if (!found)
                throw todo_file_exception("Unknown command: '" + command + "'.");

            if (command == "subtask") {
                std::cout << "\nSubcommands:\n";
                #define X(command_name, subcommand, description) \
                    if (command == command_name) \
                        std::cout << "  " << subcommand << " - " << description << '\n';
                TODO_SUBCOMMANDS
                #undef X

                std::printf("\n   Flags:\n");
                #define X(command_name, subcommand, flag, alias, description) \
                    if (command == command_name) \
                        std::cout << "    " << subcommand << " " << flag << " (" << alias << ") - " << description << ".\n";
                TODO_SUBCOMMANDS_FLAGS
                #undef X
            }

            bool has_flags = false;
            #define X(command_name, flag, alias, desc) \
                if (command == command_name) { \
                    if (!has_flags) \
                        std::cout << "Flags:\n"; \
                    std::cout << "  " << flag << " (" << alias << ") --> " << desc << std::endl; \
                    has_flags = true; \
                }
            TODO_FLAGS
            #undef X
        }

    public:
        using basic_command::basic_command;

        bool run() override {
            if (flags.size() <= 0)
                show_all_commands();
            else if (has_flag(flags, "--specify", "-sp")) {
                if (flags.size() <= 1)
                    throw todo_file_exception("Expected an command after '--specify' flag.");

                specify_command(flags[1]);
            } 
            else
                dialog::report(report_types::error, "Unexpected flag: %s", flags[0].c_str());
                
            
            return true;
        }
    };

    class version_command: public basic_command {
    public:
        using basic_command::basic_command;

        bool run() override {
            if (!flags.empty())
                throw todo_file_exception("Usage: todo version");

            dialog::report(report_types::success, "Todofile CLI todo manager - version: %s", TODO_VERSION);
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

        bool in_list(const std::vector<std::string>& list1, const std::vector<std::string>& list2) {
            for (std::size_t i = 0; i < list1.size(); ++i)
                for (std::size_t j = 0; j < list2.size(); ++j)
                    if (list1[i] == list2[j])
                        return true;

            return false;
        }

        std::vector<std::string> arguments;

    public:
        using basic_command::basic_command;

        bool run() override {
            arguments.push_back(std::string("checkable"));
            arguments.push_back(std::string("readable"));
            arguments.push_back(std::string("editable"));
            arguments.push_back(std::string("nosql"));


            if (flags.empty()) {
                WFile file(CONFIG_FILE);
                std::cout << file.read();
                return true;
            }
            if (flags.size() != 2)
                throw todo_file_exception("Usage: todo config <checkable|readable|editable|nosql> <true|false>");

            if (!in_list(flags, arguments))
                throw todo_file_exception("Config option must be valid.");

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
            dialog::report(report_types::success, "'%s' set to %s.", flags[0].c_str(), flags[1].c_str());
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
                throw todo_file_exception("Usage: todo count [--checked|--unchecked|--logs|--subtasks]");

            TODOConfig config;
            std::size_t count = config.read_count();
            if (!flags.empty()) {
                if (has_flag(flags, "--checked", "-c")) {
                    const auto tasks = read_tasks();
                    validate_task_count(tasks, count);
                    count = 0;
                    for (const auto& task : tasks)
                        count += task.checked ? 1 : 0;
                } 
                else if (has_flag(flags, "--unchecked", "-un")) {
                    const auto tasks = read_tasks();
                    validate_task_count(tasks, count);
                    count = 0;
                    for (const auto& task : tasks)
                        count += task.checked ? 0 : 1;
                } 
                else if (has_flag(flags, "--logs", "-l")) {
                    count = 0;
                    for (const auto& i: fs::directory_iterator(LOG_DIRECTORY))
                        if (i.is_regular_file() && i.path().extension() == ".log")
                            ++count;
                } 
                else if (has_flag(flags, "--subtasks", "-st")) {
                    std::unique_ptr<subtask_handler> handler;
                    if (config.read_config().nosql)
                        handler = std::make_unique<subtask_nosql_handler>();
                    else
                        handler = std::make_unique<subtask_sql_handler>();
                    const auto st = handler->read_subtasks();
                    count = st.size();
                }
                else {
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
                dialog::report(report_types::error, "No argument was given.");
                return false;
            }

            if (flags.size() <= 1) {
                dialog::report(report_types::error, "Too few arguments.");
                return false;
            }

            if (flags.size() != 2) {
                dialog::report(report_types::error, "Too many arguments!");
                return false;
            }

            TODOConfig c;
            const auto& config = c.read_config();

            if (!config.editable) {
                dialog::report(report_types::error, "Not allowed to edit tasks!");
                return config.editable;
            }


            WFile file = WFile(TODO_FILE);

            int index = std::stoi(flags[0]);

            if (index <= 0) {
                dialog::report(report_types::error, "Wrong index was given: %d", index);
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

    class restore_command: public basic_command {
    protected:
        static void ensure_directory(const fs::path& path) {
            if (fs::exists(path)) {
                if (!fs::is_directory(path))
                    throw todo_file_exception("Repository path is not a directory: " + path.string());
                return;
            }

            fs::create_directories(path);
        }

        static void ensure_file(const char* path, const char* initial_content) {
            if (fs::exists(path))
                return;

            WFile file(path);
            file.create(path);
            file.write(initial_content);
        }

        void restore_count(bool affirmative) {
            if (!affirmative)
                return;

            ensure_directory(CONFIG_DIRECTORY);
            ensure_file(COUNTER_FILE, "0");

            const auto tasks = read_tasks();

            TODOConfig configs;
            configs.write_count(tasks.size());

            dialog::report(report_types::success, "Task count restored.");
        }    

        void restore_repo(bool affirmative) {
            if (!affirmative)
                return;

            if (!fs::is_regular_file(TODO_FILE))
                throw todo_file_exception("Repository wasn't initialized!");

            ensure_directory(TODO_REPO);
            ensure_directory(CONFIG_DIRECTORY);
            ensure_directory(LOG_DIRECTORY);
            ensure_directory(PRIORITY_DIRECTORY);
            ensure_file(CONFIG_FILE, BASIC_CONFIG);
            ensure_file(COUNTER_FILE, "0");

            const auto tasks = read_tasks();
            TODOConfig config;
            config.write_count(tasks.size());

            #ifdef _WIN32
            if (!SetFileAttributesA(TODO_REPO, FILE_ATTRIBUTE_HIDDEN))
                dialog::report(report_types::warn, "Failed attempt to hide directory.");
            #endif

            dialog::report(report_types::success, "Todofile repository restored.");
        }

        void recreate_subtasks(bool affirmative) {
            if (!affirmative)
                return;

            ensure_directory(TODO_REPO);
            ensure_directory(CONFIG_DIRECTORY);
            ensure_file(CONFIG_FILE, BASIC_CONFIG);

            TODOConfig cfig;
            const bool use_nosql = cfig.read_config().nosql;

            if (!use_nosql) {
                subtask_sql_handler h;
                const bool sql_exists = fs::exists(TODO_SUBTASKS_SQL);
                const bool nosql_exists = fs::exists(TODO_SUBTASKS_NOSQL);

                h.create_database();
                h.create_table();
                if (!sql_exists && nosql_exists) {
                    subtask_nosql_handler nosql;
                    const auto& subtasks = nosql.read_subtasks();

                    for (const auto& subtask: subtasks) 
                        h.insert_subtask(subtask);

                    if (!fs::remove(TODO_SUBTASKS_NOSQL))
                        throw todo_file_exception("Could not remove the migrated subtask database.");
                }
            } else {
                subtask_nosql_handler h;
                const bool nosql_exists = fs::exists(TODO_SUBTASKS_NOSQL);
                const bool sql_exists = fs::exists(TODO_SUBTASKS_SQL);
                std::vector<subtask> subtasks;

                if (!nosql_exists && sql_exists) {
                    subtask_sql_handler sql;
                    subtasks = sql.read_subtasks();
                }

                h.create_database();
                for (const auto& subtask: subtasks)
                    h.insert_subtask(subtask);

                if (!nosql_exists && sql_exists && !fs::remove(TODO_SUBTASKS_SQL))
                    throw todo_file_exception("Could not remove the migrated subtask database.");
            }

            dialog::report(report_types::success, "Todofile subtasks database recreated.");
        }

        void restore_configs(bool affirmative) {
            if (!affirmative)
                return;

            ensure_directory(TODO_REPO);
            ensure_directory(CONFIG_DIRECTORY);
            ensure_file(CONFIG_FILE, BASIC_CONFIG);

            WFile file = WFile(CONFIG_FILE);
            file.write(BASIC_CONFIG);
        }

        void restore_link(bool affirmative) {
            if (!affirmative)
                return;

            ensure_directory(TODO_REPO);
            ensure_directory(PRIORITY_DIRECTORY);
            
            ensure_file(LINK_FILE, "");
            ensure_file(LINK_COUNT, "0");

            link_file_handler lfh;

            const auto& pt = lfh.read_links();
            const std::size_t count = lfh.read_count();

            if (count != pt.size()) {
                WFile f(LINK_COUNT);
                f.write(std::to_string(pt.size()).c_str());

                dialog::report(report_types::success, "Todofile tasks priority count restored.");
            }

            dialog::report(report_types::success, "Todofile tasks priority repository restored.");
        }

    public:
        using basic_command::basic_command;

        bool run() {
            bool rcount     = false, 
                 rrepo      = false,
                 rsub       = false,
                 rconfig    = false,
                 rlink      = false;

            if (flags.empty()) {
                rcount  = true;
                rrepo   = true;
                rsub    = true;
                rconfig = true;
                rlink   = true;
            } 
            else {
                rcount  = has_flag(flags, "--count",   "-c");
                rrepo   = has_flag(flags, "--repo",    "-r");
                rsub    = has_flag(flags, "--sub",     "-s");
                rconfig = has_flag(flags, "--config",  "-cf");
                rlink   = has_flag(flags, "--link",    "-l");

                for (const auto& flag : flags) {
                    if (flag != "--count" && flag != "-c" &&
                        flag != "--repo" && flag != "-r" &&
                        flag != "--sub" && flag != "-s" &&
                        flag != "--config" && flag != "-cf" &&
                        flag != "--link" && flag != "-l") 
                    {
                        throw todo_file_exception("Unexpected restore option: '" + flag + "'");
                    }
                }
            }

            restore_repo(rrepo);
            restore_configs(rconfig);
            restore_count(rcount);
            recreate_subtasks(rsub);
            restore_link(rlink);

            std::string holder;

            holder = std::string("Repository was successfully restored with: ");
            if (rcount)  holder += "count | ";
            if (rrepo)   holder += "directories | ";
            if (rsub)    holder += "subtask database | ";
            if (rconfig) holder += "config file reseted | ";
            if (rlink)   holder += "task priorities restored |";
            holder += ";";

            const char* message = logger.log_message(holder);
            logger.generate(message);

            return true;
        }
    };

    class subtask_command: public basic_command {
    private:
        TODOConfig config;
        std::size_t count = 0;

        std::unique_ptr<subtask_handler> handler;

        /**
         * Expected command line: 
         *      todo subtask add "<ID>" "<DESC>"
         * <ID> --> task id
         * <DESC> --> subtask description
         */
        bool add() {
            const unsigned int id = parse_task_number(flags[1]);
            count = config.read_count();

            auto tasks = read_tasks();
            validate_task_count(tasks, count);

            auto argument = join_arguments(flags, 2);

            if (id > count)
                throw todo_file_exception("Task number does not exist.");

            subtask st;
            
            st = subtask_form(id, argument, (has_flag(flags, "--done", "-d")));

            int result = handler->insert_subtask(st);

            return (result <= 0) ? false : true;
        }   

        /**
         * Expected command line: 
         *      todo subtask remove "<ID>" 
         * <ID> --> subtask ID
         */
        bool remove() {
            const unsigned int id = parse_task_number(flags[1]);
            int result = handler->remove_subtask(id);

            return (result <= 0) ? false : true;
        }

        /**
         * Expected command line: 
         *      todo subtask list <FLAGS>
         * <FLAGS> ->
         *  ? --all | -a   <TASK_ID>
         *  ? --id  | -i   <SUBTASK_ID>
         *  ? --priority | -p <PRIORITY>
         *  ? --status | -s <STATUS>
         */
        bool list() {
            if (has_flag(flags, "--all", "-a")) {
                if (flags.size() < 3)
                    throw todo_file_exception("No Todofile task ID was provided.");

                const unsigned int id  = parse_task_number(flags[2]);
                const auto& subtasks   = handler->read_subtasks(id);

                for (const auto& i: subtasks) 
                    show_info(i);
            } 
            else if (has_flag(flags, "--id", "-i")) {
                if (flags.size() < 3)
                    throw todo_file_exception("No subtask ID was provided.");

                const unsigned int id  = parse_task_number(flags[2]);
                const auto& subtask    = handler->read_subtask(id);

                show_all_info(subtask);
            } 
            else if (has_flag(flags, "--priority", "-p")) {
                if (flags.size() < 3)
                    throw todo_file_exception("No priority value was provided.");

                std::string priority = flags[2];
                const auto& subtasks = handler->read_subtasks();
                const auto& fetched  = fetch_priority(subtasks, subtask::to_priority(priority.c_str()));

                if (fetched.size() <= 0)
                    dialog::report(report_types::warn, "No subtasks was found with '%s' priority.", priority.c_str());

                for (const auto& i: fetched) 
                    show_info(i);
            }
            else if (has_flag(flags, "--status", "-s")) {
                if (flags.size() < 3)
                    throw todo_file_exception("No status value was provided.");

                std::string status   = flags[2];
                const auto& subtasks = handler->read_subtasks();
                const auto& fetched  = fetch_status(subtasks, subtask::to_status(status.c_str()));

                if (fetched.size() <= 0)
                    dialog::report(report_types::warn, "No subtasks was found with '%s' status.", status.c_str());

                for (const auto& i: fetched) 
                    show_all_info(i);
            }
            else {
                const auto& subtasks = handler->read_subtasks();

                if (subtasks.size() <= 0)
                    dialog::report(report_types::warn, "No subtasks were cataloged.");

                for (const auto& i: subtasks) 
                    show_info(i);
            }

            return true;
        }

        /**
         * Expected command line:
         *      todo subtask edit <ID> <NEW_DESC>
         * <ID> --> subtask ID
         * <NEW_DESC> --> new description ('--' if don't want to change that)
         */
        bool edit() {
            const auto& c = config.read_config();

            if (!c.editable) {
                dialog::report(report_types::error, "Not allowed to edit subtasks!");
                return c.editable;
            }

            const unsigned int parsed_subtask_id = parse_task_number(flags[1]);
            if (parsed_subtask_id > static_cast<unsigned int>(std::numeric_limits<int>::max()))
                throw todo_file_exception("Subtask ID is too large.");

            const int subtask_id = static_cast<int>(parsed_subtask_id);
            subtask original = handler->read_subtask(subtask_id);
            count = config.read_count();
            const auto tasks = read_tasks();
            validate_task_count(tasks, count);

            std::cout << "Enter '--' to keep the current value.\n";
            
            std::string description = flags[2];
            if (description == "--")
                description = original.subtask;

            std::cout << "Task ID: ";
            std::string holder;
            std::cin >> holder;

            unsigned int task_id = static_cast<unsigned int>(original.task_id);
            if (holder != "--") {
                task_id = parse_task_number(holder);
                if (task_id > count)
                    throw todo_file_exception("Task number does not exist.");
            }

            std::cout << "Priority: ";
            std::string p;
            std::cin >> p;

            if (p == "--")
                p = subtask::stringfy(original.priority);

            std::cout << "Status: ";
            std::string s;
            std::cin >> s;

            if (s == "--")
                s = subtask::stringfy(original.status);

            std::cout << "Due date: ";
            std::string dd;
            std::cin >> dd;
            if (dd == "--")
                dd = original.due_date;

            original.task_id = static_cast<int>(task_id);
            original.subtask = description;
            original.priority = subtask::to_priority(p.c_str());
            original.due_date = dd;
            original.status = subtask::to_status(s.c_str());

            subtask updated = handler->edit_subtask(subtask_id, original);

            show_all_info(updated);

            return true;
        }

        std::vector<subtask> fetch_priority(const std::vector<subtask>& subtasks, const subtask::subtask_priority priority) {
            std::vector<subtask> result;

            dialog::report(report_types::debug, "Priority type: %s", subtask::stringfy(priority));

            for (const auto& i: subtasks) 
                if (i.priority == priority) 
                    result.push_back(i);

            return result;
        }

        std::vector<subtask> fetch_status(const std::vector<subtask>& subtasks, const subtask::subtask_status status) {
            std::vector<subtask> result;

            for (const auto& i: subtasks) 
                if (i.status == status) 
                    result.push_back(i);

            return result;
        }
        
    public:
        /**
         * Expected command line: 
         *      todo subtask "<SUBCOMMAND>" 
         * <SUBCOMMAND> --> add/remove/list/edit
         */


        subtask_command(const std::vector<std::string>& arguments): basic_command(arguments) {
            const auto& c = this->config.read_config();

            if (c.nosql)
                this->handler = std::make_unique<subtask_nosql_handler>();
            else
                this->handler = std::make_unique<subtask_sql_handler>();
        }

        bool run() override {

            if (flags[0] == "add") {
                if (flags.size() < 2) 
                    throw todo_file_exception("Too few arguments."); 

                this->add();
            }
            else if (flags[0] == "remove") {
                if (flags.size() < 2) 
                    throw todo_file_exception("Too few arguments.");

                this->remove();
            } else if (flags[0] == "list") {
                this->list();
            } else if (flags[0] == "edit")  {
                if (flags.size() < 2)
                    throw todo_file_exception("No subtask ID was provided.");

                this->edit();
            }
            else {
                throw todo_file_exception("Unknown subcommand called '" + flags[0] + "'.");
            }

            return true;
        }
    };

    class priority_command: public basic_command {
    private:
        enum class priority {
            NONE    = 0,
            LOW     = 1,
            MEDIUM  = 2,
            HIGH    = 3,
            URGENT  = 4
        };

        link_file_handler lfh;

        const std::string stringfy(priority p) {
            switch (p) {
                case priority::NONE:            return "none";
                case priority::LOW:             return "low";
                case priority::MEDIUM:          return "medium";
                case priority::HIGH:            return "high";
                case priority::URGENT:          return "urgent";
                default:                        throw todo_file_exception("Unknown type of priority.");
            }
        }

        priority to_priority(const std::string& str) {
            std::string p = "";

            for (const auto& i: str) 
                p += std::toupper(i);
            
            if (p == "NONE")        return priority::NONE;
            if (p == "LOW")         return priority::LOW;
            if (p == "MEDIUM")      return priority::MEDIUM;
            if (p == "HIGH")        return priority::HIGH;
            if (p == "URGENT")      return priority::URGENT;

            throw todo_file_exception("Unknow type of priority.");
        }

        bool verify(int* id) {
            TODOConfig c;
            const auto previous = c.read_count();

            if (*id <= 0) {
                dialog::report(report_types::error, "Wrong index was given: %d", *id);
                return false;
            }

            if (*id > previous) {
                dialog::report(report_types::error, "Task of index %d does not exists.", *id);
                return false;
            }

            return true;
        }

        void new_priority(priority p, const int task_id) {
            WFile f = WFile(LINK_FILE);
            const std::size_t count = lfh.read_count();

            if (count < 0)
                throw todo_file_exception("Priority counter file was obstructed: " + std::string("count is negative."));

            this->validate_if_task_exists(this->read_tasks(), task_id);

            const auto& pt = lfh.read_links();

            if (count != pt.size())
                throw todo_file_exception("Priority counter file was obstructed: " + std::string("count does not match actual priorities count."));
            
            for (const auto& i: pt) {
                if (i.task_id == task_id) {
                    if (i.priority == (int) p) {
                        dialog::report(report_types::warn, "Task priority is already set to '%s'", stringfy(p).c_str());
                        return;
                    }

                    std::string holder = std::string(std::to_string(i.task_id) + ":" + std::to_string((int) p));
                    f.print(holder.c_str(), i.line);

                    std::string log = std::string("Priority on task " + std::to_string(i.task_id) + " was changed from '" + 
                        stringfy((priority_command::priority) i.priority) + "' to '" + stringfy((priority_command::priority) p) + "'.");

                    dialog::report(report_types::success, log.c_str());

                    const auto msg = this->logger.log_message(log);
                    this->logger.generate(msg);

                    return;
                }
            }

            // Exemplos: 
            //      1:3
            //      2:1
            //      3:0
            std::string holder = std::string(std::to_string(task_id) + ":" + std::to_string((std::size_t) p));

            f.append(holder.c_str());
            lfh.increment_count();

            dialog::report(report_types::success, "Priority '%s' was added to task %d", stringfy(p).c_str(), task_id);
        }

    public:
        using basic_command::basic_command;

        bool run() override {
            if (flags.size() < 3) 
                throw todo_file_exception("Too few arguments.");

            const auto& subcmd = flags[0];
            int id = std::stoi(flags[1]);
            const auto priority = to_priority(flags[2]);

            if (!verify(&id))
                return false;

            if (subcmd == "new") {
                new_priority(priority, id);
            }
            else {
                dialog::report(report_types::error, "No such subcommand exists: %s", subcmd.c_str());
                return false;
            }

            return true;
        }
    };
}