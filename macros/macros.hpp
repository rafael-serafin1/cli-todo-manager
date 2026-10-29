#ifndef MACROS_H
#define MACROS_H

#define TODO_COMMANDS                                                                                       \
    X("init",        "Initializes a new repo", init_command)                                                \
    X("list",        "List 20 first todo tasks", list_command)                                              \
    X("add",         "Adds a new task", add_command)                                                        \
    X("remove",      "Removes a task", remove_command)                                                      \
    X("switch",      "Checks/Unchecks a task", switch_command)                                              \
    X("help",        "List all avaliable commands", help_command)                                           \
    X("version",     "Show project's version", version_command)                                             \
    X("count",       "Show amount of tasks", count_command)                                                 \
    X("config",      "Configure Todofile options", config_command)                                          \
    X("edit",        "Edit an task by its index", edit_command)                                             \
    X("check",       "Checks if repository/Todofile is obstructed", check_command)                          \
    X("restore",     "Restore Todofile repository", restore_command)                                        \
    X("subtask",     "Adds a subtask to an existing task", subtask_command)                                 \
    X("priority",    "Adds priority to an Todofile task", priority_command)                                 \
    X("fetch",       "Fetch task by id or priority", fetch_command)

#define TODO_SUBCOMMANDS                                                                                                                        \
    X("subtask", "add <TASK_ID> \"<SUBTASK_DESC>\"",    "Adds a new subtask to the database")                                                   \
    X("subtask", "remove <SUBTASK_ID>",                 "Removes a subtask from the database")                                                  \
    X("subtask", "list",                                "Show all subtasks recorded")                                                           \
    X("subtask", "edit <ID> \"<NEW_DESC>\"",            "Edits an existing task (Use \"--\" in <NEW_DESC> to avoid change its description)")    

#define TODO_SUBCOMMANDS_FLAGS                                                                                                          \
    X("subtask", "add",  "--done",                      "-d",   "Declare an subtask as already done")                                   \
    X("subtask", "list", "--all <TASK_ID>",             "-a",   "Show every single subtask related to that specific task")              \
    X("subtask", "list", "--id  <SUBTASK_ID>",          "-i",   "Search for that specific subtask by its ID")                           \
    X("subtask", "list", "--priority \"<PRIORITY>\"",   "-p",   "Fetch all subtasks by its priority")                                   \
    X("subtask", "list", "--status \"<STATUS>\"",       "-s",   "Fetch all subtasks by its status")                                     \

#define TODO_FLAGS                                                                              \
    X("help",    "--specify",    "-sp",     "Specifies an command")                             \
    X("list",    "--logs",       "-l",      "List all logs registed")                           \
    X("list",    "--all",        "-a",      "List all tasks")                                   \
    X("list",    "--checked",    "-c",      "List all checked todo tasks")                      \
    X("list",    "--unchecked",  "-un",     "List all unchecked todo tasks")                    \
    X("list",    "--priorities", "-p",      "List all priorities")                              \
    X("add",     "--done",       "-d",      "Mark task as already done")                        \
    X("count",   "--checked",    "-c",      "Count all checked tasks")                          \
    X("count",   "--unchecked",  "-un",     "Count all unchecked tasks")                        \
    X("count",   "--logs",       "-l",      "Count all log files")                              \
    X("count",   "--subtasks",   "-st",     "Count all subtasks")                               \
    X("count",   "--priorities", "-p",      "Count all priorities registred")                   \
    X("restore", "--repo",       "-r",      "Restore repository only")                          \
    X("restore", "--count",      "-c",      "Restore count file only")                          \
    X("restore", "--sub",        "-s",      "Restore subtasks database")                        \
    X("restore", "--config",     "-cf",     "Reset configuration to defaults")                  \
    X("restore", "--link",       "-l",      "Restore priority repo only")                       \
    X("fetch",   "--priority",   "-p",      "Fetch tasks by priority")                          \
    X("fetch",   "--all",        "-a",      "Fetch a task by id and all subtasks related")      


#ifdef _WIN32
    #define TODO_REPO           ".todo\\"
    #define CONFIG_DIRECTORY    ".todo\\config"
    #define LOG_DIRECTORY       ".todo\\logs"
    #define PRIORITY_DIRECTORY  ".todo\\link"

    #define CONFIG_FILE     ".todo\\config\\config.bin"
    #define COUNTER_FILE    ".todo\\config\\count.bin\0"

    #define LINK_FILE       ".todo\\link\\link.bin"
    #define LINK_COUNT      ".todo\\link\\count.bin"

    #define TODO_FILE       ".\\Todofile\0"
#else
    #define TODO_REPO           ".todo/"
    #define CONFIG_DIRECTORY    ".todo/config"
    #define LOG_DIRECTORY       ".todo/logs"
    #define PRIORITY_DIRECTORY  ".todo/link"

    #define CONFIG_FILE     ".todo/config/config.bin"
    #define COUNTER_FILE    ".todo/config/count.bin\0"

    #define LINK_FILE       ".todo/link/LINK.bin"
    #define LINK_COUNT      ".todo/link/COUNT.bin"

    #define TODO_FILE       "./Todofile\0"
#endif

#endif