#include "config.hpp"

#include <charconv>
#include <limits>
#include <string>
#include <system_error>

namespace TODO {
    std::size_t TODO::TODOConfig::read_count() {
        WFile counter_file(count_path);
        const std::string content = counter_file.read();
        std::size_t count = 0;
        const auto result = std::from_chars(
            content.data(), content.data() + content.size(), count
        );

        if (content.empty() || result.ec != std::errc{} ||
            result.ptr != content.data() + content.size()) {
            throw todo_file_exception("Invalid task count in count.bin.");
        }

        return count;
    }

    void TODO::TODOConfig::write_count(std::size_t count) {
        WFile counter_file(count_path);
        const std::string content = std::to_string(count);
        counter_file.write(content.c_str());
    }

    void TODO::TODOConfig::increment_count() {
        const std::size_t count = read_count();
        if (count == std::numeric_limits<std::size_t>::max())
            throw todo_file_exception("Task count is too large.");
        write_count(count + 1);
    }

    void TODO::TODOConfig::decrement_count() {
        const std::size_t count = read_count();
        if (count == 0)
            throw todo_file_exception("Cannot decrement an empty task count.");
        write_count(count - 1);
    }

    TODO::Todofile_Config TODO::TODOConfig::read_config() {
        if (configs == nullptr) {
            configs = new Todofile_Config{false, false};
        }

        file_writer = new WFile(this->config_path);

        std::string content = file_writer->read();
        auto config = Todofile_Config{false, false};

        if (!content.empty()) {
            config.checkable = (content.find("checkable=true") != std::string::npos);
            config.readable = (content.find("readable=true") != std::string::npos);
        }

        delete file_writer;
        file_writer = nullptr;

        *configs = config;
        return config;
    }

    void TODO::TODOConfig::set_config(TODO::TODOConfig config) {
        (void)config;
    }
};