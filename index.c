#include <stdlib.h>

#include "macros.h"
#include "utils/utils.h"
#include "main/parse.h"

extern Status all_files(void);
extern int master(CLI cli);
extern Status todofile_add_task(const char *task);
extern CLI *cli;

Status _todo_help(list todo) {
    message(MSG_INFO, "All commands avaliable to usage (todo <COMMAND>):");
    #define X(cmd, usage)       \
        message(MSG_INFO, "\t - %s --> %s.", cmd, usage);
        __EXPLICIT_TODO_CMDS
    #undef X
    return SUCCESS;
}

Status _todo_version() {
    message(MSG_INFO, "Todofile version: \'%s\'", __TODO_VERSION);
    return SUCCESS;
}

Status _todo_init(char **argv) {
    int exit = system("mkdir \".todo/\"");

    if (exit != 0) {
        message(MSG_ERROR, "Repository already exists.");
        return FAILURE;
    }

    all_files();

    return SUCCESS;
}

Status _todo_config() {
    int exit = master(*cli);
    if (exit != 0) 
        return FAILURE;
    return SUCCESS;
}

Status _todo_add(string todo) {
    return todofile_add_task(todo);
}

Status _todo_remove(string todo) {
    return SUCCESS;
}

Status _todo_check(string todo) {
    return SUCCESS;
}

Status _todo_list() {
    return SUCCESS;
}