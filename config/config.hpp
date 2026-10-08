#pragma once
#include "../file/file.hpp"

#ifndef CONFIG_H
#define CONFIG_H

#include <cstddef>

#ifdef _WIN32
    #define CONFIG_FILE ".todo\\config\\config.bin"
    #define COUNTER_FILE ".todo\\config\\count.bin\0"
    #define TODO_FILE ".\\Todofile\0"
#else
    #define CONFIG_FILE ".todo/config/config.bin"
    #define COUNTER_FILE ".todo/config/count.bin\0"
    #define TODO_FILE "./Todofile\0"
#endif

namespace TODO {
    struct Todofile_Config {
        bool checkable;
        bool readable;
    };

    class TODOConfig {
    private:
        Todofile_Config* configs = nullptr;
        WFile* file_writer       = nullptr;
        const char* count_path   = COUNTER_FILE;
        const char* config_path  = CONFIG_FILE;

        void write_count(std::size_t count);

    public:    
        ~TODOConfig() {
            if (file_writer != nullptr)
                delete file_writer;
        }
        
        std::size_t read_count();
        void increment_count();
        void decrement_count();
        void set_config(TODO::TODOConfig config);
        void save_config();
        TODO::Todofile_Config read_config();
    };
}

#endif