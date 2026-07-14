#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <locale.h>

#include "utils.h"

list initialize_list(void) {
    list L = (list) malloc(sizeof(struct linked_list));

    if (L == NULL)
        return NULL;

    L->item = NULL;
    L->next = NULL;

    return L;
}

void add_to_list(list L, const char *flag) {
    while (L->next != NULL)
        L = L->next;

    L->item = (char *) malloc(strlen(flag) + 1);
    strcpy(L->item, flag);

    L->next = (list) malloc(sizeof(list));

    L->next->item = NULL;
    L->next->next = NULL;
}

size_t list_items_count(list L) {
    register size_t count = 0;
    while (L != NULL) {
        ++count;
        L = L->next;
    } 
    return count;
}

#define ANSI_RESET      "\x1b[0m"

#define ANSI_RED        "\x1b[31m"
#define ANSI_GREEN      "\x1b[32m"
#define ANSI_YELLOW     "\x1b[33m"
#define ANSI_BLUE       "\x1b[34m"
#define ANSI_MAGENTA    "\x1b[35m"
#define ANSI_CYAN       "\x1b[36m"
#define ANSI_WHITE      "\x1b[37m"

void message(MessageType type, const char *fmt, ...)
{
    const char *color;
    const char *prefix;

    switch (type)
    {
        case MSG_INFO:
            color = ANSI_BLUE;
            prefix = "[INFO]";
            break;

        case MSG_WARNING:
            color = ANSI_YELLOW;
            prefix = "[WARNING]";
            break;

        case MSG_ERROR:
            color = ANSI_RED;
            prefix = "[ERROR]";
            break;

        case MSG_SUCCESS:
            color = ANSI_GREEN;
            prefix = "[SUCCESS]";
            break;

        case MSG_DEBUG:
            color = ANSI_MAGENTA;
            prefix = "[DEBUG]";
            break;

        default:
            color = ANSI_WHITE;
            prefix = "[MESSAGE]";
            break;
    }

    printf("%s%s ", color, prefix);

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("%s\n", ANSI_RESET);
}

/// @brief configura localização atual
/// @param void
void configurar_locale(void) {
    #if defined(_WIN32)
    system("chcp 65001 > nul");
    #endif

    const char *locais[] = {
        "pt_BR.UTF-16",
        "pt_BR.utf16",
        "Portuguese_Brazil.1252",
        "Portuguese",
        ""
    };
    register int i; 
    for (i = 0; i < 5; i++) {
        const char *r = setlocale(LC_ALL, locais[i]);
        if (r != NULL) {
            printf("Locale ativo: %s\n", r);
            return;
        }
        free((void *) r);
    }
    printf("Aviso: Locale não pôde ser configurado.\n");
}

int flag_prefix(const char* _arg) {
    if (_arg == NULL)
        return 0;

    if (_arg[0] == '-') {
        if (_arg[1] == '-')  
            return 1;

        if (_arg[1] != '\0') 
            return 1;
    }

    return 0;
}

void fgoto(FILE *_f, const int _line) {
    rewind(_f);

    if (_line <= 1)
        return;

    int current = 1;
    int c;

    while ((c = fgetc(_f)) != EOF) {
        if (c == '\n') {
            current++;

            if (current == _line)
                return;
        }
    }
}