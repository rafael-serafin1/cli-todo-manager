#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "../utils/utils.h"

#ifndef TODOFILE_H
#define TODOFILE_H

#define CHECKBOX_UNCHECKED "[ ]"
#define CHECKBOX_CHECKED "[X]"

#define LIST_FLAGS_                 \
    X("--all", "-a")                \
    X("--checked", "-c")            \
    X("--unchecked", "-un")

typedef struct __todo_task {
    int count;
    CHECKED isChecked;
    char task[__MAX_SIZE_BUFFER];
} Todo;

void create_todofile(void);
void create_todofile_inPath(const char *_Path);

Status todofile_add_task(const char* task);
Status todofile_remove_task(const int _index);
Status todofile_list_task(list _flags);
Status todofile_check_task(const int _index);
Status todofile_uncheck_task(const int _index);

#endif