#pragma once
#include "utils/utils.h"
#include "main/parse.h"
#define string const char*

#ifndef MACROS_H
#define MACROS_H

Status _todo_help(list todo);
Status _todo_version();
Status _todo_count();
Status _todo_init(char **argv);
Status _todo_config();
Status _todo_add(string todo);
Status _todo_remove(const int index);
Status _todo_check(const int index);
Status _todo_uncheck(const int index);
Status _todo_ongoing(const int index);
Status _todo_reset(int argc, char **argv);
Status _todo_list(list _flags);

#define __TODO_VERSION "0.2.2"
#define __TODO_CMDS         \
    X("help")               \
    X("version")            \
    X("count")              \
    X("init")               \
    X("config")             \
    X("add")                \
    X("remove")             \
    X("check")              \
    X("uncheck")            \
    X("ongoing")            \
    X("reset")              \
    X("list")


#define __TODO_CMDS_FUNC                    \
    X("help", _todo_help(flags))            \
    X("version", _todo_version())           \
    X("count", _todo_count())               \
    X("init", _todo_init(argv))             \
    X("config", _todo_config())             \
    X("add", _todo_add(task))               \
    X("remove", _todo_remove(index))        \
    X("check", _todo_check(index))          \
    X("uncheck", _todo_uncheck(index))      \
    X("ongoing", _todo_ongoing(index))      \
    X("reset", _todo_reset(argc, argv))     \
    X("list", _todo_list(flags))         

#define __EXPLICIT_TODO_CMDS                                                \
    X("help", "Show this message")                                          \
    X("version", "Show project's version")                                  \
    X("count", "Show tasks count")                                          \
    X("config <CONFIG>", "Configure Todofile options")                      \
    X("add \"<TASK>\"", "Adds a task to Todofile")                          \
    X("remove \"<INDEX>\"", "Removes a task from Todofile by index")        \
    X("check \"<INDEX>\"", "Check a task from Todofile by it index")        \
    X("uncheck \"<INDEX>\"", "Uncheck a task from Todofile by it index")    \
    X("ongoing \"<INDEX>\"", "Sets a task as currently being done")         \
    X("reset <FLAGS>", "Reset current tasks by specification")              \
    X("list <FLAGS>", "List first 20 tasks from Todofile")        


#pragma region

#define ALL_FLAGS                       \
    X("--readable", "-r")               \
    X("--checkable", "-c")              \
    X("--visible", "-v")                \
    X("--all", "-a")                    \
    X("--checked", "-c")                \
    X("--unchecked", "-un")             \
    X("--ongoing", "-on")

/// @brief CONFIG COMMAND FLAGS
#define CONFIG_FLAGS                                                                    \
    X("--readable", "-r", "Makes Todofile be storaged as text file, not binary.")       \
    X("--checkable", "-c", "Makes tasks in Todofile checkable.")                        \
    X("--visible", "-v", "Makes Todofile storaged at any local.")                     

#define LIST_FLAGS                                                  \
    X("--all", "-a", "List all tasks.")                             \
    X("--checked", "-c", "List all tasks that are checked.")        \
    X("--unchecked", "-un", "List all tasks that are unchecked.")   \
    X("--ongoing", "-on", "List all tasks that are ongoing.")

#define RESET_FLAGS                                                 \
    X("--empty", "-e", "Empty tasks.")                              \
    X("--all", "-a", "Remove all task statuses. ('X' | '-')")       

#pragma endregion
#endif