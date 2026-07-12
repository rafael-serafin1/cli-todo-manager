#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "../utils.h"
#include "count.h"

typedef struct __todo_task {
    int count;
    CHECKED isChecked;
    char task[__MAX_SIZE_BUFFER];
} Todo;

void todofile_add_task() {
    
}