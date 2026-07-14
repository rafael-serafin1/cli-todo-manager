#include <ctype.h>
#include "parse.h"

extern int flag_prefix(const char* _arg);

/// @brief parse flags given by usability on command
/// @param argc number of arguments
/// @param argv arguments
/// @return Status -> SUCCESS or FAILURE
Status parse_flags(int argc, char **argv) {
    if (argc < 3) 
        return SUCCESS;
    
    CLI helper = *cli;
    for (register int i = 0; i < argc; ++i) {
        #define X(flag, alias, desc)           \
            if ((strcmp(flag, argv[i]) == 0 || strcmp(alias, argv[i]) == 0) && (flag_prefix(argv[i]) == 1)) { add_to_list(helper.flags, argv[i]); }
            CONFIG_FLAGS
        #undef X
    }

    if (helper.flags->item == NULL && flag_prefix(argv[2]) == 1) {
        message(MSG_ERROR, "No such flag called: \'%s\'", argv[2]);
        return FAILURE;
    } else
        return SUCCESS;
    cli->flags = helper.flags; 
    return SUCCESS;
}

/// @brief parse command given
/// @param argc number of arguments
/// @param argv arguments
/// @return Status -> SUCCESS or FAILURE
Status parse_command(int argc, char **argv) {
    string cmd = cli->command;
    list flags = cli->flags;
    
    Status S = parse_flags(argc, argv);
    string task = (flag_prefix(argv[2]) == 0) ? argv[2] : "None";

    int index = 0;
    for (register int i = 0; i < argc; ++i)
        index = parse_index(argv[i]);

    #define X(use, fun)                \
        if (strcmp(cmd, use) == 0) return fun;
        __TODO_CMDS_FUNC
    #undef X

    message(MSG_ERROR, "Todofile (todo): Inexistent or incorrect command: \"%s\"\n", cmd);
    return FAILURE;
}

int parse_index(const char* _arg) {
    if (_arg == NULL || *_arg == '\0')
        return -1;

    int result = 0;

    while (*_arg) {
        if (!isdigit((unsigned char)* _arg))
            return -1;

        result = result * 10 + (*_arg - '0');
        ++_arg;
    }

    return result;
}