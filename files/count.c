#include "count.h"

int counter = 0;
int* global = &counter;
int final_count = 0;

void create_counter_file() {
    FILE *cf = fopen(COUNTER_FILE, "wb");

    fwrite(global, integer, 1, cf);

    fclose(cf);
}

/// @brief Reads counter file 
/// @return negative number if 'failed otherwise an positive number
int read_counter_file() {
start:
    FILE *f = fopen(COUNTER_FILE, "rb");
    
    if (f == NULL) {
        fclose(f);
        create_counter_file();
        goto start;
    }

    char buffer[2];
    fread(buffer, sizeof(buffer), 1, f);

    int repo = (int) buffer[0];

    message(MSG_INFO, "Actual value storaged: \'%d\'", repo);

    fclose(f);
    return repo;
}

int verify_counter_file() {
start:
    FILE *f = fopen(COUNTER_FILE, "rb");
    int repo;

    if (f = NULL)
        repo = -1;
    else 
        repo = 0;    
    fclose(f);

    if (repo == 0)
        repo = read_counter_file();
    else {
        create_counter_file();
        goto start;
    }

    message(MSG_DEBUG, "Incremented value: \'%d\'", (repo + 1));
    return repo;
}

void increment_counter_file() {
    counter = verify_counter_file();
    FILE *cf = fopen(COUNTER_FILE, "wb");

    ++counter;
    fwrite(global, sizeof(int), 1, cf);

    fclose(cf);
}

void reset_counter_file() {
    final_count = counter;
    counter = 0;
    FILE *cf = fopen(COUNTER_FILE, "wb");

    fwrite(global, integer, 1, cf);

    fclose(cf);
    message(MSG_DEBUG, "Final count: \'%d\'", final_count);
    return;
}