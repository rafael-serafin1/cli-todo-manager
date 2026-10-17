#ifndef MACROS_H
#define MACROS_H

#define TODO_COMMANDS                                                                                   \
    X("init",    "Initializes a new repo", init_command)                                                \
    X("list",    "List all todo tasks", list_command)                                                   \
    X("add",     "Adds a new task", add_command)                                                        \
    X("remove",  "Removes a task", remove_command)                                                      \
    X("switch",  "Checks/Unchecks a task", switch_command)                                              \
    X("help",    "List all avaliable commands", help_command)                                           \
    X("version", "Show project's version", version_command)                                             \
    X("count",   "Show amount of tasks", count_command)                                                 \
    X("config",  "Configure Todofile options", config_command)                                          \
    X("edit",    "Edit an task by its index", edit_command)                                             \
    X("check",   "Checks if repository/Todofile is obstructed", check_command)                          \
    X("restore", "Restore Todofile repository", restore_command)                                        \
    X("subtask", "Adds a subtask to an existing task", subtask_command)

#define TODO_FLAGS                                               \
    X("list",    "--logs",       "-l")                           \
    X("list",    "--all",        "-a")                           \
    X("list",    "--checked",    "-c")                           \
    X("list",    "--unchecked",  "-un")                          \
    X("add",     "--done",       "-d")                           \
    X("count",   "--logs",       "-l")                           \
    X("subtask", "--done",       "-d")                           \
    X("restore", "--repo",       "-r")                           \
    X("restore", "--count",      "-c")                           \
    X("restore", "--sub",        "-s")                           


#ifdef _WIN32
    #define TODO_REPO           ".todo\\"
    #define CONFIG_DIRECTORY    ".todo\\config"
    #define LOG_DIRECTORY       ".todo\\logs"
    #define PRIORITY_DIRECTORY  ".todo\\link"

    #define CONFIG_FILE     ".todo\\config\\config.bin"
    #define COUNTER_FILE    ".todo\\config\\count.bin\0"
    #define TODO_FILE       ".\\Todofile\0"
#else
    #define TODO_REPO           ".todo/"
    #define CONFIG_DIRECTORY    ".todo/config"
    #define LOG_DIRECTORY       ".todo/logs"
    #define PRIORITY_DIRECTORY  ".todo/link"

    #define CONFIG_FILE     ".todo/config/config.bin"
    #define COUNTER_FILE    ".todo/config/count.bin\0"
    #define TODO_FILE       "./Todofile\0"
#endif

#endif