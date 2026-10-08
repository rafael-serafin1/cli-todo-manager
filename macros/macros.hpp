#ifndef MACROS_H
#define MACROS_H

#define TODO_COMMANDS                                           \
    X("init", "Initializes a new repo", init_command)           \
    X("list", "List all todo tasks", list_command)              \
    X("add", "Adds a new task", add_command)                    \
    X("remove", "Removes a task", remove_command)               \
    X("switch", "Checks/Unchecks a task", switch_command)       \
    X("help", "List all avaliable commands", help_command)      \
    X("version", "Show project's version", version_command)     \
    X("count", "Show amount of tasks", count_command)           \
    X("config", "Configure Todofile options", config_command)

#endif