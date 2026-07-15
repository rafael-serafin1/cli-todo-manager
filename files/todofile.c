#include "todofile.h"

#define BUFFER 2048

#define CHECK_ERROR(reason)         \
    message(MSG_ERROR, "Couldn't check task \"%d\" :: \'%s\'", _index, reason);
#define NULL_POINTER_EXCEPTION  \
    if (todo == NULL) { message(MSG_ERROR, "Couldn't open file: \"%s\"", path); return FAILURE; } 

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

/// @brief responsible for adding a task to Todofile
/// @param task string containing task
/// @return Status -> SUCCESS OR FAILURE
Status todofile_add_task(const char *task) {
    __todo_config CONFIGS = read_config_file();
    int count = read_counter_file();
    const char* mode = (CONFIGS.readable == verdade) ? "a\0" : "ab\0";
    const char* path = (CONFIGS.visible == verdade) ? TODO : TODO_FILE;

    if (count < 0) {
        message(MSG_ERROR, "Error in counter file. Returned number: \"%d\"", count);
        return FAILURE;
    }

    char buffer[BUFFER];

    if (CONFIGS.checkable == verdade)
        snprintf(buffer, sizeof(buffer), "%d. %s %s\n", (count + 1), CHECKBOX_UNCHECKED, task);
    else 
        snprintf(buffer, sizeof(buffer), "%d. %s\n", (count + 1), task);

    //message(MSG_DEBUG, "Mensagem final: %s", buffer);

    FILE *todo = fopen(path, mode);
    NULL_POINTER_EXCEPTION

    if (CONFIGS.readable == verdade) 
        fputs(buffer, todo);
    else 
        fwrite(buffer, sizeof(char), strlen(buffer), todo);

    increment_counter_file();
    fclose(todo);
    return SUCCESS;
}

/// @brief responsible for removing a task from Todofile
/// @param _index task index
/// @return Status -> SUCCESS OR FAILURE
Status todofile_remove_task(const int _index) {
    __todo_config CONFIGS = read_config_file();
    int count = read_counter_file();
    const char* mode = (CONFIGS.readable == verdade) ? "r+\0" : "rb+\0";
    const char* path = (CONFIGS.visible == verdade) ? TODO : TODO_FILE;

    if (_index <= 0) {
        message(MSG_ERROR, "Invalid value for index: \"%d\"", _index);
        return FAILURE;
    }

    if (count < 0) {
        message(MSG_ERROR, "Error in counter file. Returned number: \"%d\"", count);
        return FAILURE;
    }
    
    FILE *todo = fopen(path, mode);
    NULL_POINTER_EXCEPTION

    message(MSG_DEBUG, "Index: %d", _index);
    fgoto(todo, _index);
    
    char buffer[BUFFER];

    if (fgets(buffer, sizeof(buffer), todo) != NULL) {
        register size_t tmnh = strlen(buffer);
        if (buffer[tmnh - 1] == '\n') {
            // mantenha desse jeito, pois é necessário que a quebra de linha se mantenha
            buffer[tmnh - 1] == '\0';
            --tmnh;
        }

        for (register size_t i = 0; i < tmnh; i++)
            buffer[i] = ' ';
        
        fseek(todo, -tmnh, SEEK_CUR);
        fputs(buffer, todo);
    }
    

    fclose(todo);
    return SUCCESS;
}

/// @brief List all tasks from Todofile
/// @param _flags list containing all flags
/// @return Status -> SUCCESS OR FAILURE
Status todofile_list_task(list _flags) {
    __todo_config CONFIGS = read_config_file();
    register int count = read_counter_file();
    const char* mode = (CONFIGS.readable == verdade) ? "r\0" : "rb\0";
    const char* path = (CONFIGS.visible == verdade) ? TODO : TODO_FILE;


    list helper = _flags;
    int hasAllFlag = 0, hasCheckedFlag = 0, hasUncheckedFlag = 0;
    
    while (helper->item != NULL) {
        if (strcmp(helper->item, "--all") == 0 || strcmp(helper->item, "-a") == 0)
            hasAllFlag = 1;
        if (strcmp(helper->item, "--checked") == 0 || strcmp(helper->item, "-c") == 0) 
            hasCheckedFlag = 1;
        if (strcmp(helper->item, "--unchecked") == 0 || strcmp(helper->item, "-un") == 0) 
            hasUncheckedFlag = 1;

        helper = helper->next;
    }


    if (hasCheckedFlag == 1 && hasUncheckedFlag == 1) {
        message(MSG_ERROR, "Can't use \'--checked\' and \'--unchecked\' flags together.");
        return FAILURE;
    }

    if (count <= 0) {
        message(MSG_INFO, "No tasks recorded.\n");
        return FAILURE;
    }


    register int handler = 0;
    if (count > 20)
        if (hasAllFlag == 1) 
            handler = count;
        else
            handler = 20;
    else
        handler = count;


    FILE *todo = fopen(path, mode);
    NULL_POINTER_EXCEPTION

    char buffer[BUFFER];

    for (register size_t i = 0; i < handler; ++i) {
        fgoto(todo, (i + 1));
        if (fgets(buffer, sizeof(buffer), todo) == NULL)
            break;

        clear_buffer(buffer);
        message(MSG_INFO, "Task %d: \"%s\"", (i + 1), buffer);
    }
    
    if (hasAllFlag == 1) 
        message(MSG_SUCCESS, "All tasks sucessfully shown.");
    else
        message(MSG_SUCCESS, "20 first tasks sucessfully shown.");

    fclose(todo);
    return SUCCESS;
}

/// @brief If configs allow checking tasks, check a task
/// @param _index check a task by its index
/// @return Status -> SUCCESS OR FAILURE
Status todofile_check_task(const int _index) {
    __todo_config CONFIGS = read_config_file();
    
    if (CONFIGS.checkable == falso) {
        CHECK_ERROR("Todofile configs won't allow tasks to be checkable. Use \'todo config --checkable\'.")
        return FAILURE;
    }

    register int count = read_counter_file();
    const char* mode = (CONFIGS.readable == verdade) ? "r+\0" : "rb+\0";
    const char* path = (CONFIGS.visible == verdade) ? TODO : TODO_FILE;

    FILE *todo = fopen(path, mode);
    NULL_POINTER_EXCEPTION

    char buffer[BUFFER];
    int c;

    fgoto(todo, _index);
    while ((c = fgetc(todo)) != EOF) {
        if (c == '\n') {
            CHECK_ERROR("Task obstructed or removed.")
            return FAILURE;
        }

        if (c == '[') {
            c = fgetc(todo);

            if (c == EOF) {
                CHECK_ERROR("End of the file reached.")
                return FAILURE;
            }

            if (c == 'X') {
                CHECK_ERROR("Task was already checked.")
                return FAILURE;
            }

            if (c != ' ') {
                CHECK_ERROR("Invalid task format.")
                return FAILURE;
            }

            fseek(todo, -1, SEEK_CUR);
            fputc('X', todo);
            break;
        }
    }

    fstart(todo);
    fgets(buffer, sizeof(buffer), todo);

    clear_buffer(buffer);
    message(MSG_SUCCESS, "Task succesfully checked: \'%s\'", buffer);

    fclose(todo);
    return SUCCESS;
}

/// @brief 
/// @param _index 
/// @return 
Status todofile_uncheck_task(const int _index) {
    __todo_config CONFIGS = read_config_file();
    
    if (CONFIGS.checkable == falso) {
        CHECK_ERROR("Todofile configs won't allow tasks to be checkable. Use \'todo config --checkable\'.")
        return FAILURE;
    }

    register int count = read_counter_file();
    const char* mode = (CONFIGS.readable == verdade) ? "r+\0" : "rb+\0";
    const char* path = (CONFIGS.visible == verdade) ? TODO : TODO_FILE;

    FILE *todo = fopen(path, mode);
    NULL_POINTER_EXCEPTION

    char buffer[BUFFER];
    int c;

    fgoto(todo, _index);
    while ((c = fgetc(todo)) != EOF) {
        if (c == '\n') {
            CHECK_ERROR("Task obstructed or removed.")
            return FAILURE;
        }

        if (c == '[') {
            c = fgetc(todo);

            if (c == EOF) {
                CHECK_ERROR("End of the file reached.")
                return FAILURE;
            }

            if (c == ' ') {
                CHECK_ERROR("Can't uncheck an already unchecked task.")
                return FAILURE;
            }

            fseek(todo, -1, SEEK_CUR);
            fputc(' ', todo);
            break;
        }
    }

    fstart(todo);
    fgets(buffer, sizeof(buffer), todo);

    clear_buffer(buffer);
    message(MSG_SUCCESS, "Task succesfully unchecked: \'%s\'", buffer);

    fclose(todo);
    return SUCCESS;
}