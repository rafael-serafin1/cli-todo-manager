#include "config.h"

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
    __todo_config t;
    
    if (cf == NULL) {
        message(MSG_ERROR, "No repository found. Use \'todo init\'.");
        create_config_file();
        goto start;
    }

    fread(&t, sizeof(__todo_config), 1, cf);

    fclose(cf);
    return t;
}

int _read_config_file(__todo_config *t) {
    FILE *cf = fopen(CONFIG_FILE, "rb");

    if (cf == NULL)
        return 0;

    fread(t, sizeof(*t), 1, cf);
    fclose(cf);

    return 1;
}