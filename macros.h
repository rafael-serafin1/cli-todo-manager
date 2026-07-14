#pragma once
#include "utils/utils.h"
#include "main/parse.h"
#define string const char*

#ifndef MACROS_H
#define MACROS_H

Status _todo_help(list todo);
Status _todo_version();
Status _todo_init(char **argv);
Status _todo_config();
Status _todo_add(string todo);
Status _todo_remove(const int index);
Status _todo_check(string todo);
Status _todo_uncheck(string todo);
Status _todo_list();

#define __TODO_VERSION "0.0.1"
#define __TODO_CMDS         \
    X("help")               \
    X("version")            \
    X("init")               \
    X("config")             \
    X("add")                \
    X("remove")             \
    X("check")              \
    X("list")


#define __TODO_CMDS_FUNC                \
    X("help", _todo_help(flags))        \
    X("version", _todo_version())       \
    X("init", _todo_init(argv))         \
    X("config", _todo_config())         \
    X("add", _todo_add(task))           \
    X("remove", _todo_remove(index))    \
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
    X("--readable", "-r", "Makes Todofile be storaged as text file, not binary.")     \
    X("--checkable", "-c", "Makes tasks in Todofile checkable.")                      \
    X("--visible", "-v", "Makes Todofile storaged at any local.")                     

#pragma endregion
#endif