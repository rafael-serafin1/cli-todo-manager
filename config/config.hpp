#pragma once
#include "../file/file.hpp"
#include "../macros/macros.hpp"

#ifndef CONFIG_H
#define CONFIG_H

#include <cstddef>

namespace TODO {
    struct Todofile_Config {
        bool checkable;
        bool readable;
        bool nosql;
    };

    class TODOConfig {
    private:
        Todofile_Config* configs = nullptr;
        WFile* file_writer       = nullptr;
        const char* count_path   = COUNTER_FILE;
        const char* config_path  = CONFIG_FILE;

    public:    
        ~TODOConfig() {
            if (file_writer != nullptr)
                delete file_writer;
        }
        
        std::size_t read_count();

        void increment_count();
        void increment_count(const int amount);

        void decrement_count();
        void decrement_count(const int amount);

        void set_config(TODO::TODOConfig config);
        TODO::Todofile_Config read_config();

        void write_count(std::size_t count);
    };
}

#endif