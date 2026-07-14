#include "todofile.h"

extern __todo_config read_config_file();
extern void create_config_file();
extern void write_config_file(__todo_config config);
extern void create_counter_file();
extern int read_counter_file();
extern void increment_counter_file();

void create_todofile() {
    __todo_config CONFIGS = read_config_file();
    const char *mode = (CONFIGS.readable == verdade) ? "w\0" : "wb\0";

    if (CONFIGS.visible == verdade) {
        FILE *todofile = fopen(TODO, mode);

        fclose(todofile);
    } else {
        FILE *todofile = fopen(TODO_FILE, mode);

        fclose(todofile);
    }

    return;
}

void create_todofile_inPath(const char *_Path) {
    __todo_config cf = read_config_file();
    const char *mode = (cf.readable == verdade) ? "w\0" : "wb\0";

    if (remove("Todofile") != 0) {
        message(MSG_ERROR, "Error during Todofile exclusion.");
        return;
    }

    FILE *f = fopen(_Path, mode);
    fclose(f);
}

Status todofile_add_task(const char *task) {
    __todo_config CONFIGS = read_config_file();
    int count = read_counter_file();
    const char* mode = (CONFIGS.readable == verdade) ? "a\0" : "ab\0";

    if (count < 0) {
        message(MSG_ERROR, "Error in counter file. Returned number: \"%d\"", count);
        return FAILURE;
    }

    char buffer[__MAX_SIZE_BUFFER];

    if (CONFIGS.checkable == verdade)
        snprintf(buffer, sizeof(buffer), "%d. %s %s\n", (count + 1), CHECKBOX_UNCHECKED, task);
    else 
        snprintf(buffer, sizeof(buffer), "%d. %s\n", (count + 1), task);

    //message(MSG_DEBUG, "Mensagem final: %s", buffer);

    FILE *todo;
    if (CONFIGS.readable == verdade) {
        todo = fopen(TODO, mode);
        fputs(buffer, todo);
    } else {
        todo = fopen(TODO_FILE, mode);
        fwrite(buffer, sizeof(char), strlen(buffer), todo);
    }

    increment_counter_file();
    fclose(todo);
    return SUCCESS;
}
