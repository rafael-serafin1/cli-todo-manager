#pragma once
#include <stdio.h>
#include <string.h>
#include "./config/config.h"

#define CQL_VERSION "0.0.1"

#define PRINTVERSION() \
    printf("%s", CQL_VERSION);
#define PRINTNEWLINE() \
    printf("\n");

// estou apenas prometendo ao compilador que isso vai existir
Status add_todo(char* todo);
Status remove_todo(char* todo);
Status list_todo(void);
Status version(void);
Status build(char* file_local);
Status error_handle(void);

// tabela com string, função e adicional
#define CQL_COMMANDS                                                \
    X(CMD_VERSION, s = version(), PRINTNEWLINE())                   \
    X(CMD_HELP, s = help(), PRINTNEWLINE())                         \
    X(CMD_ADD, s = add_todo(supplier), supplier = argv[2])          \
    X(CMD_REMOVE, s = remove_todo(supplier), supplier = argv[2])    \
    X(CMD_LIST, s = list_todo(), PRINTNEWLINE())                    \
    X(CMD_ERROR, s = error_handle(), PRINTNEWLINE())                \

// define um enum através da coluna 'string' tabela
#define X(string, function, additional) string,
typedef enum {
    CQL_COMMANDS
} Command;
#undef X;

// tabela dos comandos disponíveis
#define COMMANDS                                                     \
    X("todo --help", "Shows this command list.")                     \
    X("todo --version", "Shows TODO terminal CLI version.")          \
    X("todo add \"\"", "Add a task to your TODO list.")              \
    X("todo remove \"\"", "Removes a task from your TODO list.")     \
    X("todo list", "Lists every single task in TODO list.")


/**
 * * funções dos comandos
 */
Status version() {
    printf("TODO terminal list version: %s\n", CQL_VERSION);
    return SUCCESS;
}

/// @brief 
/// @return enum Status --> FAILURE == (2)
Status error_handle() {
    printf("Ocorreu um erro ao executar o comando.\n");
    return FAILURE;
}

Status help() {
    printf("TODO terminal list CLI, todos os comandos listado a seguir: ");

    #define X(command, usage) command, usage
        printf("%s: %s", COMMANDS, COMMANDS);
    #undef X;

    return define_status(0);
}

/// @brief adds a task to todo list
/// @param todo - string containing a object to add
/// @return enum Status
Status add_todo(char* todo) {
    printf("Deseja adicionar a tarefa '%s' à sua lista de tarefas? (y/N): ", todo);
    char new = ask();

    if (new == 'N' || new == 'n') {
        printf("Você escolheu não adicionar a tarefa '%s' à sua lista\n", new);
        return define_status(1);
    }

    printf("Adicionando tarefa: %s\n", todo);

    const char* script_py = "./python/add.py";
    char command[256];

    snprintf(command, sizeof(command), "python %s \"%s\"", script_py, todo);

    register int f = system(command);

    free(script_py);
    free(todo);
    if (f != 0) return define_status(1);
    return define_status(0);
}

/// @brief removes a task from todo list
/// @param todo - string containing a object to remove 
/// @return enum Status
Status remove_todo(char* todo) {
    printf("Deseja remover a tarefa '%s' da sua lista de tarefas (y/N): ", todo);
    char new = ask();

    if (new == 'N' || new == 'n') {
        printf("Você escolheu não remover a tarefa '%s' da sua lista\n", new);
        return define_status(1);
    }

    printf("Removendo a tarefa: %s\n", todo);

    const char* script_py = "./python/remove.py";
    char command[256];

    snprintf(command, sizeof(command), "python %s %s", script_py, todo);
    register int f = system(command);

    free(script_py);
    free(todo);
    if (f != 0) return define_status(1);
    return define_status(0);
}


Status list_todo(void) {
    const char* py = "./python/list.py";
    char command[256];

    snprintf(command, sizeof(command), "python %s", py);
    register int f = system(command);

    free(py);
    if (f != 0) return define_status(1);
    return define_status(0);
}