#include <stdio.h>
#include <stdlib.h>
#include "utils/utils.h"
#include "cmd.h"

char* supplier = NULL;

Command parse_cmd(char* cmd_str) {
    if (strcmp(cmd_str, "add") == 0) return CMD_ADD;
    if (strcmp(cmd_str, "remove") == 0) return CMD_REMOVE;
    if (strcmp(cmd_str, "list") == 0) return CMD_LIST;
    if (strcmp(cmd_str, "--version") == 0) return CMD_VERSION;
    if (strcmp(cmd_str, "--help") == 0) return CMD_HELP;
    else printf("Comando inexistente: '%s'", cmd_str);
    return CMD_ERROR;
}

int main(int argc, char *argv[]) {
    configurar_locale();

    register int i;
    if (argc < 2) {
        printf("Uso: todo <comando>\nUse todo --help para saber mais.\n");
        return 1;
    }

    char *command_str = argv[1];
    trim(command_str);
    Command cmd = parse_cmd(command_str);

    #define X(string, function, additional) \
        case string: {additional; i = function;} break;
        switch (cmd) {
            CQL_COMMANDS;
        }
    #undef X;
    
    free(supplier);
    return 0;
}