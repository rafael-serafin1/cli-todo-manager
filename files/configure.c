#include "count.h"
#include "config.h"
#include "macros.h"

#include "../main/parse.h"

#define CONFIGURE _configure_file(argv[1])

extern __todo_config read_config_file();
extern void create_todofile();
extern void create_todofile_inPath(const char *_Path);

Status all_files() {
    create_config_file();
    create_counter_file();
    create_todofile();
    return SUCCESS;
}

Status change_configs(int _size, char **_argv) {
    __todo_config CONFIGS = read_config_file();
    register int count = 0;

    for (register int i = 0; i < _size; ++i) {
        #define X(flag, alias, usage)   \
            if (strcmp(flag, _argv[i]) == 0 || strcmp(alias, _argv[i]) == 0) { usage; ++count; continue; }
            CONFIG_FLAGS_USAGE
        #undef X
    }
    
    if (count == 0) {
        message(MSG_INFO, "Actual configuration: ");
        message(MSG_INFO, "\tReadable: '%d' | Checkable: '%d' | Visible: '%d'", CONFIGS.readable, CONFIGS.checkable, CONFIGS.visible);

        return SUCCESS;
    }
    
    __todo_config cf = read_config_file();
    if (cf.readable == falso)
        create_todofile_inPath(TODO_FILE);

    message(MSG_DEBUG, "NEW CONFIG => READABLE: '%d' CHECKABLE: '%d' VISIBLE: '%d'", cf.readable, cf.checkable, cf.visible);
    return SUCCESS;
}

int master(CLI cli) {
    size_t items = list_items_count(cli.flags);

    if (items == 0) {
        message(MSG_WARNING, "No configuration provided: \'%s\'", cli.command);
        return -1;
    }

    char **argv;
    argv = (char **) malloc(items * sizeof(*argv));

    struct linked_list helper;
    helper = *(cli.flags);

    for (register size_t i = 0; helper.next != NULL; ++i) {
        argv[i] = (char *) malloc(strlen(helper.item) + 1);
        strcpy(argv[i], helper.item);

        message(MSG_DEBUG, "Flag: %s", helper.item);
        helper = *(helper.next);
    }

    __todo_config CONFIGS; 
    if (_read_config_file(&CONFIGS) == 0) {
        message(MSG_ERROR, "There is not such repository for access. Use \"todo init\"");
        return 1;
    }

    if (cli.flags->item != NULL) 
        message(MSG_DEBUG, "OLD CONFIG => READABLE: '%d' CHECKABLE: '%d' VISIBLE: '%d'", CONFIGS.readable, CONFIGS.checkable, CONFIGS.visible);
    Status S = change_configs(items, argv);

    for (register int i = 0; i < items; ++i)
        free((void *) argv[i]);
    return 0;
}