#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "macros.h"
#include "utils/utils.h"
#include "main/parse.h"

extern int master(CLI cli);
CLI *cli;            // variável global do arquivo 'parse.h'

int main(int argc, char **argv) {
    Status S;

    if (argc < 2) {
        message(MSG_ERROR, "No command provided. Correct usage -> \'todo <COMMAND>\'\n");
        return 1;
    }

    cli = (CLI *) malloc(sizeof(CLI));   
    strcpy(cli->command, argv[1]);

    cli->flags = initialize_list();

    S = parse_command(argc, argv);

    message(MSG_DEBUG, "CLI -- Command: %s | Flags: %s", cli->command, cli->flags->item);

    free(cli);
    return 0;
}