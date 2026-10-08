#pragma once

#include <exception>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "../index.hpp"

#define MAX_SIZE_BUFFER 2456

namespace TODO {
    class WFile {
    protected:
        FILE* _file = nullptr;
        std::string _path;
        std::string _mode;

        bool ends_with(const std::string& str, const std::string& suffix) {
            return str.size() >= suffix.size() &&
                str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
        }

        void module(const char* mode) {
            _mode = mode;
            find_mode();
        }

        void open() {
            _file = fopen(_path.c_str(), _mode.c_str());
        }

        void close() {
            if (_file != nullptr) {
                fclose(_file);
                _file = nullptr;
            }
        }

        void check() {
            if (_file == nullptr)
                throw TODO::todo_file_exception("Não foi possível abrir o arquivo.");
        }

    public:
        explicit WFile(const char* path) : _path(path) { }

        void find_mode();
        std::string find_mode(const char* path);

        void write(const char* _Content);
        void append(const char* _Content);
        std::string read();
        std::string read(const int _Line);

        std::vector<std::string> read_lines();

        void create(const char* _Path);
    };
}