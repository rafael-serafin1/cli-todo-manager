#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../macros.h"
#include "../utils/utils.h"

#ifndef PARSE_H
#define PARSE_H

#define __MIN_ARRAY_SIZE 256

typedef struct __TODO_CLI__ {
    char command[__MIN_ARRAY_SIZE];
    list flags;
} CLI;

extern CLI *cli;

Status parse_flags(int argc, char **argv);
Status parse_command(int argc, char **argv);
int parse_index(const char *_arg);

#endif