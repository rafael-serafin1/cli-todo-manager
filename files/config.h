#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef CONFIG_H
#define CONFIG_H

#ifdef _WIN32
    #include <direct.h>
#else
    #include <sys/stat.h>
#endif

#include "../utils/utils.h"

#define TODO "Todofile\0"

#ifdef _WIN32
    #define CONFIG_FILE ".todo\\config\\config.bin"
    #define COUNTER_FILE ".todo\\config\\count.bin\0"
    #define TODO_FILE ".todo\\Todofile\0"
#else
    #define CONFIG_FILE ".todo/config/config.bin"
    #define COUNTER_FILE ".todo/config/count.bin\0"
    #define TODO_FILE ".todo/Todofile\0"
#endif


#define __MAX_SIZE_BUFFER 32768
#define __MIN_SIZE_BUFFER 256

typedef enum {
    falso = 0,
    verdade = 1
} BOOLEAN;

typedef enum __todo_check {
    CHECK = 0,
    UNCHECK = 1
} CHECKED;

typedef struct {
    BOOLEAN readable;
    BOOLEAN checkable;
    BOOLEAN visible;
} __todo_config;

void write_config_file(__todo_config config);
void create_config_file();
__todo_config read_config_file();
int _read_config_file(__todo_config *t);

#endif