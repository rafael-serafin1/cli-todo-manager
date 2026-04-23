#pragma once
#include <stdio.h>
#include <string.h>

typedef enum {
    SUCCESS,
    WARNING,
    FAILURE
} Status;

Status define_status(int boolean) {
    if (boolean == 0) return SUCCESS;
    else return FAILURE;
}