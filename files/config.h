#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <direct.h>
#else
    #include <sys/stat.h>
#endif

#include "../utils.h"

#define TODO "Todofile\0"
#define CONFIG_FILE ".todo\\config\\config.bin\0"
#define COUNTER_FILE ".todo\\config\\count.bin\0"

#define __MAX_SIZE_BUFFER 2560
#define __MIN_SIZE_BUFFER 256

typedef enum {
    falso = 0,
    verdade = 1
} BOOLEAN;

typedef enum {
    CHECK = 0,
    UNCHECK = 1
} CHECKED;

typedef struct {
    BOOLEAN readable;
    BOOLEAN checkable;
    BOOLEAN visible;
} __todo_config;

void write_config_file(__todo_config config) {
    FILE *cf = fopen(CONFIG_FILE, "wb");

    fwrite(&config, sizeof(__todo_config), 1, cf);

    fclose(cf);
}

void create_config_file() {
    register int result = system("mkdir \".todo/config/\"");
    if (result != 0) goto error_path;

    if (result == 0) {
        FILE *file = fopen(CONFIG_FILE, "wb");
    
        __todo_config config = {falso, falso, verdade};
    
        fwrite(&config, sizeof(__todo_config), 1, file);
        fclose(file);
    } else
    error_path:
        message(MSG_ERROR, "Repository already exists.");
    
    return;
}

__todo_config read_config_file() {
start:
    FILE *cf = fopen(CONFIG_FILE, "rb");
    
    if (cf == NULL) {
        fclose(cf);
        create_config_file();
        goto start;
    }

    __todo_config t;
    fread(&t, sizeof(__todo_config), 1, cf);

    fclose(cf);

    return t;
}