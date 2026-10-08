#include <string.h>
#include <utility>

#include "file.hpp"
#include "../index.hpp"

namespace TODO {
    void WFile::find_mode() {
        if (ends_with(_path, ".bin"))
            _mode += "b";
    }

    std::string WFile::find_mode(const char* path) {
        if (ends_with(path, ".bin"))
            return "b";
        return "";
    }

    void WFile::write(const char* _Content) {
        module("w");
        open();
        check();

        if (_Content != nullptr)
            fwrite(_Content, sizeof(char), strlen(_Content), _file);

        close();
    }

    std::string WFile::read() {
        module("r");
        open();
        check();

        std::string content;
        char buffer[MAX_SIZE_BUFFER];

        while (size_t bytes = fread(buffer, 1, sizeof(buffer), _file))
            content.append(buffer, bytes);

        close();
        return content;
    }



    std::string WFile::read(const int _Line) {
        if (_Line < 1)
            throw TODO::todo_file_exception("Line must be greater than 0.");

        module("r");
        open();
        check();

        std::string content;
        char buffer[MAX_SIZE_BUFFER];
        int currentLine = 1;

        while (currentLine <= _Line && fgets(buffer, sizeof(buffer), _file) != nullptr) {
            std::string line(buffer);
            if (currentLine == _Line) {
                content = line;
                break;
            }
            ++currentLine;
        }

        close();

        if (currentLine < _Line) {
            throw TODO::todo_file_exception("Line does not exist.");
        }

        if (!content.empty() && content.back() == '\n')
            content.pop_back();

        if (!content.empty() && content.back() == '\r')
            content.pop_back();

        return content;
    }

    std::vector<std::string> WFile::read_lines() {
        std::vector<std::string> content;
        const std::string file_content = read();
        std::size_t start = 0;

        while (start < file_content.size()) {
            const std::size_t end = file_content.find('\n', start);
            const std::size_t line_end = end == std::string::npos
                ? file_content.size()
                : end;

            std::string line = file_content.substr(start, line_end - start);
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            content.push_back(std::move(line));

            if (end == std::string::npos)
                break;
            start = end + 1;
        }
        return content;
    }

    void WFile::append(const char* content) {
        module("a");
        open();
        check();

        if (content != nullptr && strlen(content) > 0)
            fwrite(content, sizeof(char), strlen(content), _file);

        if (content == nullptr || strlen(content) == 0 || content[strlen(content) - 1] != '\n')
            fwrite("\n", sizeof(char), 1, _file);

        close();
    }

    void WFile::create(const char* _Path) {
        const std::string mode = "w" + find_mode(_Path) + "x";
        FILE* f = fopen(_Path, mode.c_str());

        if (f == NULL) 
            throw TODO::todo_file_exception("File already exists!");

        fclose(f);
    }
};