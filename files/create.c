#include "count.h"
#include "config.h"
#include "macros.h"
#include "todofile.h"

#define CONFIGURE _configure_file(argv[1])

int _configure_file(const char *task) {
    #define X(flag, usage)  \
        if (strcmp(flag, task) == 0) { usage; }
        CONFIG_FLAGS_USAGE
    #undef X
}

int main(int argc, char **argv) {
    if (argc < 2) {
        message(MSG_ERROR, "No command provided.");
        return -1;
    }

    if (strcmp(argv[1], "--readable") == 0) CONFIGURE;
    else if (strcmp(argv[1], "--checkable") == 0) CONFIGURE;
    else if (strcmp(argv[1], "--visible") == 0) CONFIGURE;

    __todo_config CONFIGS = read_config_file();
    message(MSG_DEBUG, "READABLE: '%d'\tCHECKABLE: '%d'\tVISIBLE: '%d'", CONFIGS.readable, CONFIGS.checkable, CONFIGS.visible);

    return 0;
}