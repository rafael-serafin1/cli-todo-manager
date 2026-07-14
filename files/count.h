#pragma once
#include <stdio.h>
#include <stdlib.h>

#ifndef COUNT_H
#define COUNT_H

#include "../utils/utils.h"
#define COUNTER_FILE ".todo\\config\\count.bin\0"

#define integer sizeof(int)

extern int counter;
extern int* global;
extern int final_count;

void create_counter_file();
int read_counter_file();
int verify_counter_file();
void increment_counter_file();
void reset_counter_file();

#endif