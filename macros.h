#pragma once
#include "utils.h"
#define string const char*

Status _todo_help(list todo);
Status _todo_version();
Status _todo_config();
Status _todo_add(string todo);
Status _todo_remove(string todo);
Status _todo_check(string todo);
Status _todo_uncheck(string todo);
Status _todo_list();

#define __TODO_VERSION "0.0.1"
#define __TODO_CMDS         \
    X("help")               \
    X("version")            \
    X("config")             \
    X("add")                \
    X("remove")             \
    X("check")              \
    X("list")


#define __TODO_CMDS_FUNC                \
    X("help", _todo_help(flags))        \
    X("version", _todo_version())       \
    X("config", _todo_config(flags))    \
    X("add", _todo_add(task))           \
    X("remove", _todo_remove(task))     \
    X("check", _todo_check(task))       \
    X("list", _todo_list())         

#define __EXPLICIT_TODO_CMDS                                        \
    X("help", "Show this message")                                  \
    X("version", "Show project's version")                          \
    X("config <CONFIG>", "Configure Todofile options")              \
    X("add <TASK>", "Adds a task to Todofile")                      


#pragma region

/// @brief FLAGS
#define CONFIG_FLAGS                                                            \
    X("--readable", "Makes Todofile be storaged as text file, not binary.")     \
    X("--checkable", "Makes tasks in Todofile checkable.")                      \
    X("--visible", "Makes Todofile storaged at any local.")                     

#pragma endregion