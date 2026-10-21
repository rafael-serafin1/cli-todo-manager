#pragma once

#include "../../../config/config.hpp"
#include "../../../utils/dialog.hpp"

#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace TODO {
    struct subtask {
        enum class subtask_priority {
            none        = 0,
            low         = 1,
            medium      = 2,
            high        = 3,
            urgent      = 4,
            inexistent  = 5
        };

        enum class subtask_status {
            pending         = 0,
            in_progress     = 1,
            completed       = 2,
            on_hold         = 3,
            cancelled       = 4,
            inexistent      = 5
        };

        int id;
        int task_id;

        std::string subtask;
        subtask_priority priority;
        
        std::string date_impl;
        std::string due_date;
        std::string date_compl;

        subtask_status status;

        static subtask_priority to_priority(const char* value) {
            std::string val = std::string(value);

            for (char& c : val)
                c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));

            subtask_priority p = subtask_priority::inexistent;
            if (val == "NONE")      p = subtask_priority::none;
            if (val == "LOW")       p = subtask_priority::low;
            if (val == "MEDIUM")    p = subtask_priority::medium;
            if (val == "HIGH")      p = subtask_priority::high;
            if (val == "URGENT")    p = subtask_priority::urgent;

            
            if (p == subtask_priority::inexistent) {
                std::string hold = value;
                hold += " doesn't exists as a valid status type.";
                throw todo_file_exception(hold);
            }

            return p;
        }

        static subtask_status to_status(const char* value) {
            std::string val = std::string(value);

            for (char& c : val)
                c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));

            subtask_status p = subtask_status::inexistent;
            if (val == "PENDING")       p = subtask_status::pending;
            if (val == "IN_PROGRESS")   p = subtask_status::in_progress;
            if (val == "COMPLETED")     p = subtask_status::completed;
            if (val == "ON_HOLD")       p = subtask_status::on_hold;
            if (val == "CANCELLED")     p = subtask_status::cancelled;

            if (p == subtask_status::inexistent) {
                std::string hold = value;
                hold += " doesn't exists as a valid status type.";
                throw todo_file_exception(hold);
            }

            return p;
        }

        static const char* stringfy(subtask_priority p) {
            switch (p) {
                case subtask_priority::none:        return "none";
                case subtask_priority::low:         return "low";
                case subtask_priority::medium:      return "medium";
                case subtask_priority::high:        return "high";
                case subtask_priority::urgent:      return "urgent";
                default:                            throw std::invalid_argument("Invalid subtask priority.");
            }
        }

        static const char* stringfy(subtask_status p) {
            switch (p) {
                case subtask_status::pending:       return "pending";
                case subtask_status::in_progress:   return "in_progress";
                case subtask_status::completed:     return "completed";
                case subtask_status::on_hold:       return "on_hold";
                case subtask_status::cancelled:     return "cancelled";
                default:                            throw std::invalid_argument("Invalid subtask priority.");
            }
        }
        
        static std::string color_output(subtask_status status) {
            const char* message = stringfy(status);
            std::string output = std::string("\x1b[3m"); 

            switch (status) {
                case subtask_status::pending:               output += dialog::output(dialog_types::dark_red, message);      break;
                case subtask_status::cancelled:             output += dialog::output(dialog_types::red, message);           break;
                case subtask_status::in_progress:           output += dialog::output(dialog_types::dark_yellow, message);   break;
                case subtask_status::completed:             output += dialog::output(dialog_types::dark_green, message);    break;
                case subtask_status::on_hold:               output += dialog::output(dialog_types::orange, message);        break;
                default:                                    throw todo_file_exception("Unexpected status.");
            }

            return output;
        }

        static std::string format_output(subtask_priority priority) {
            const char* message = stringfy(priority);
            std::string output; 

            switch (priority) {
                case subtask_priority::none:                output += message;                                                      
                    break;
                case subtask_priority::low:                 output += std::string(FAINT_ESC) + message;                             
                    break;
                case subtask_priority::medium:              output += std::string(ITALIC_ESC) + message;                            
                    break;
                case subtask_priority::high:                output += std::string(ITALIC_ESC) + std::string(BOLD_ESC) + message;    
                    break;
                case subtask_priority::urgent:              output += std::string(ITALIC_ESC) + std::string(BOLD_ESC) + std::string(UNDERLINE_ESC) +  message;
                    break;
                default:                                    throw todo_file_exception("Unexpected status.");
            }

            output += "\x1b[0m";
            return output;
        }

        std::string dump() const {
            std::string result;

            const auto& append_field = [&result](const std::string& value) {
                result += std::to_string(value.size());
                result += '\n';
                result += value;
            };

            append_field(std::to_string(id));
            append_field(std::to_string(task_id));
            append_field(subtask);
            append_field(subtask::stringfy(priority));
            append_field(date_impl);
            append_field(due_date);
            append_field(date_compl);
            append_field(subtask::stringfy(status));

            return result;
        }
    };

    subtask subtask_form(int task_id, const std::string& desc, bool status = false) {
        std::string holder;
        subtask st = subtask{};
        
        st.id      = 0;
        st.task_id = task_id;
        st.subtask = desc;

        dialog::report(report_types::unknown, "Due date: ");
        std::cin >> holder;

        st.due_date = holder;

        dialog::report(report_types::unknown, "Priority: ");
        std::cin >> holder;

        st.priority = subtask::to_priority(holder.c_str());

        if (!status)
            st.status  = subtask::subtask_status::pending;
        else
            st.status  = subtask::subtask_status::completed;

        subtask* SubTask = &st;
        return *SubTask;
    }

    void show_info(subtask st) {
        dialog::report(report_types::success, "Subtask '%d' -> '%s' | %s | %s",
            st.id, st.subtask.c_str(), subtask::format_output(st.priority).c_str(), subtask::color_output(st.status).c_str());
    }

    void show_all_info(subtask st) {
        dialog::report(report_types::success, "Subtask '%d' from task %d -> '%s' | %s | %s",
            st.id, st.task_id, st.subtask.c_str(), subtask::format_output(st.priority).c_str(), subtask::color_output(st.status).c_str());
    }

    class subtask_handler {
    protected:
        TODOConfig c = TODOConfig();
    public:
        virtual ~subtask_handler() = default;
        virtual void create_database() = 0;
        virtual void create_table() = 0;

        virtual int insert_subtask(subtask subtask) = 0;
        virtual int remove_subtask(const int subtask_id) = 0;
        virtual subtask edit_subtask(const int subtask_id, subtask now) = 0;

        virtual subtask read_subtask(int subtask_id) = 0;
        virtual std::vector<subtask> read_subtasks(int task_id) = 0;
        virtual std::vector<subtask> read_subtasks() = 0;
        
    };
}