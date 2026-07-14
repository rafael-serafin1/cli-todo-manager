#pragma once
#include <stdio.h>

#ifndef UTILS_H
#define UTILS_H

typedef enum status {
    SUCCESS = 0,
    FAILURE = 1
} Status;

typedef struct linked_list {
    char *item;
    struct linked_list* next;
} *list;

/// @brief enum for message types
typedef enum {
    MSG_INFO,
    MSG_WARNING,
    MSG_ERROR,
    MSG_SUCCESS,
    MSG_DEBUG
} MessageType;

list initialize_list(void);
void add_to_list(list L, const char *flag);
size_t list_items_count(list L);
void message(MessageType type, const char *fmt, ...);
void configurar_locale(void);
int flag_prefix(const char* _arg); 
void fgoto(FILE *_f, const int _line);

#endif

