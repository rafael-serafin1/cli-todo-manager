#include "link.hpp"
#include "../file/file.hpp"
#include "../macros/macros.hpp"

using namespace TODO;

int link_file_handler::read_count() {
    WFile f = WFile(LINK_COUNT);

    const auto& content = f.read();
    const int count = std::stoi(content);

    return count;
}

void link_file_handler::increment_count() {
    WFile f = WFile(LINK_COUNT);

    const auto& content = f.read();
    int count = std::stoi(content);

    ++count;

    f.write(std::to_string(count).c_str());
}

void link_file_handler::decrement_count() {
    WFile f = WFile(LINK_COUNT);

    const auto& content = f.read();
    int count = std::stoi(content);

    --count;

    f.write(std::to_string(count).c_str());
}

std::vector<todo_priority_task> link_file_handler::read_links() {
    std::vector<todo_priority_task> LINK;
    WFile f = WFile(LINK_FILE);
    const auto& t = f.read_lines();

    // %:%
    // % representa um número qualquer inteiro positivo
    for (std::size_t i = 0; i < t.size(); ++i) {
        const auto pos = t[i].find(':', 0);

        if (pos == std::string::npos)
            continue;

        const auto& id = t[i].substr(0, pos); 
        const auto& p  = t[i].substr(pos + 1);

        LINK.push_back(
            todo_priority_task{
                (i + 1),
                std::stoi(id),
                std::stoi(p)
            }
        );
    }

    return LINK;
}