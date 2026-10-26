#pragma once

#ifndef LINK_H
#define LINK_H

#include <iostream>
#include <vector>

namespace TODO {
    struct todo_priority_task {
        const int line;
        const int task_id;
        const int priority;
    };

    class link_file_handler {
    public:
        int read_count();
        void increment_count();
        void decrement_count();

        std::vector<todo_priority_task> read_links();
    };
}

#endif