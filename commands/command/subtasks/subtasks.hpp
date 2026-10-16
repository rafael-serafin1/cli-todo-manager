#pragma once

#include <vector>
#include <string>
#include <cctype>
#include <memory>

#include "../../../include/sqlite3.h"
#include "../../../file/file.hpp"
#include "../../../utils/dialog.hpp"

#ifdef _WIN32
    #define TODO_SUBTASKS ".todo\\subtasks.db"
#else
    #define TODO_SUBTASKS ".todo/subtasks.db"
#endif

using namespace TODO;
using namespace std;

namespace TODO {
    struct subtask {
        enum class subtask_priority {
            none,
            low,
            medium,
            high,
            urgent,
            inexistent
        };

        enum class subtask_status {
            pending,
            in_progress,
            completed,
            on_hold,
            cancelled,
            inexistent
        };

        int id;
        int task_id;

        string subtask;
        subtask_priority priority;
        
        string date_impl;
        string due_date;
        string date_compl;

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
            }

            throw std::invalid_argument("Invalid subtask priority.");
        }

        static const char* stringfy(subtask_status p) {
            switch (p) {
                case subtask_status::pending:       return "pending";
                case subtask_status::in_progress:   return "in_progress";
                case subtask_status::completed:     return "completed";
                case subtask_status::on_hold:       return "on_hold";
                case subtask_status::cancelled:     return "cancelled";
            }

            throw std::invalid_argument("Invalid subtask priority.");
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

        if (!status) {
            dialog::report(report_types::unknown, "Status: ");
            std::cin >> holder;
    
            st.status   = subtask::to_status(holder.c_str());
        } 
        else st.status  = subtask::subtask_status::completed;

        subtask* SubTask = &st;
        return *SubTask;
    }

    void show_info(subtask st) {
            dialog::report(report_types::success, "Subtask %d -> '%s' | %s | %s", 
                st.id, st.subtask.c_str(), subtask::format_output(st.priority).c_str(), subtask::color_output(st.status).c_str());
        }

    class subtask_handler {
    private:
        sqlite3* db = nullptr;

        const char* get_text(sqlite3_stmt *stmt, int column) {
            const auto* value = sqlite3_column_text(stmt, column);

            return 
                (value) 
                    ? reinterpret_cast<const char*>(value)
                    : "";
        }

        void bind_date(sqlite3_stmt* stmt, int index, const std::string& date) {
            int rc;

            if (date.empty())
                rc = sqlite3_bind_null(stmt, index);
            else
                rc = sqlite3_bind_text(stmt, index, date.c_str(), -1, SQLITE_TRANSIENT);

            if (rc != SQLITE_OK)
                throw todo_file_exception("Error binding date.");
        }

    public:
        subtask_handler() { }

        ~subtask_handler() {
            if (db)
                sqlite3_close(db);
        }

        void create_database() {
            int result = sqlite3_open(TODO_SUBTASKS, &db);
             
            if (result != SQLITE_OK) {
                sqlite3_close(db);
                throw todo_file_exception("Error during database creation.");
            }
        }

        void create_table() {
            TODO::WFile f = TODO::WFile("./commands/command/subtasks/sql/create.sql");
            char* error_message = nullptr;
        
            const auto& query = f.read();
            
            int result = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &error_message);

            if (result != SQLITE_OK) {
                sqlite3_free(error_message);  
                throw todo_file_exception("Error during subtask table creation.");
            }

            dialog::report(report_types::debug, "Table created with success!"); 
        }

        int insert_subtask(subtask subtask) {
            if (subtask.task_id <= 0)
                throw todo_file_exception("Subtasks can't have an ID below or equals 0.");
            
            sqlite3_open(TODO_SUBTASKS, &db);

            WFile f("./commands/command/subtasks/sql/insert.sql");
            auto query = f.read();

            sqlite3_stmt* rstmt = nullptr;

            if (sqlite3_prepare_v2(db, query.c_str(), -1, &rstmt, nullptr) != SQLITE_OK)
                throw todo_file_exception("Error during query preparation.");
            
            auto stmt = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>(rstmt, sqlite3_finalize);

            if (sqlite3_bind_int(stmt.get(), 1, subtask.task_id) != SQLITE_OK)
                throw todo_file_exception("Error binding task_id.");

            if (sqlite3_bind_text(stmt.get(), 2, subtask.subtask.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
                throw todo_file_exception("Error binding subtask.");

            const char* priority = subtask::stringfy(subtask.priority);

            if (sqlite3_bind_text(stmt.get(), 3, priority, -1, SQLITE_TRANSIENT) != SQLITE_OK)
                throw todo_file_exception("Error binding priority.");

            bind_date(stmt.get(), 4, subtask.due_date);
            bind_date(stmt.get(), 5, subtask.date_compl);

            const char* status   = subtask::stringfy(subtask.status);

            if (sqlite3_bind_text(stmt.get(), 6, status, -1, SQLITE_TRANSIENT) != SQLITE_OK)
                throw todo_file_exception("Error binding status.");
                
            if (sqlite3_step(stmt.get()) != SQLITE_DONE)
                throw todo_file_exception("Error inserting subtask: " + std::string(sqlite3_errmsg(db)));

            dialog::report(report_types::info, "Subtask '%s' added successfully.", subtask.subtask.c_str());

            log_generator l = log_generator();
            auto msg = l.log_message("Subtask '" + subtask.subtask + "' was removed from database.");
            l.generate(msg);

            return sqlite3_last_insert_rowid(db);
        }

        subtask read_subtask(int subtask_id) {
            if (subtask_id <= 0) 
                throw todo_file_exception("Not a valid id.");

            const char* query = R"(
                SELECT
                    subtask_id,
                    task_id,
                    subtask,
                    priority,
                    date_implemented,
                    due_date,
                    date_completed,
                    status
                FROM subtask
                WHERE subtask_id = ?;
            )";

            sqlite3_open(TODO_SUBTASKS, &db);
            sqlite3_stmt* rstmt = nullptr;

            if (sqlite3_prepare_v2(db, query, -1, &rstmt, nullptr) != SQLITE_OK) 
                throw todo_file_exception("Error during query preparation.");

            std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> stmt(rstmt, sqlite3_finalize);

            if (sqlite3_bind_int(stmt.get(), 1, subtask_id) != SQLITE_OK)
                throw todo_file_exception("Error binding subtask_id.");

            int result = sqlite3_step(stmt.get());

            if (result == SQLITE_DONE)
                throw todo_file_exception("Subtask not found.");

            if (result != SQLITE_ROW) 
                throw todo_file_exception("Too many results came back.");

            return subtask{
                sqlite3_column_int(stmt.get(), 0),

                sqlite3_column_int(stmt.get(), 1),

                get_text(stmt.get(), 2),

                subtask::to_priority(get_text(stmt.get(), 3)),

                get_text(stmt.get(), 4),

                get_text(stmt.get(), 5),

                get_text(stmt.get(), 6),

                subtask::to_status(get_text(stmt.get(), 7)),
            };
        }

        vector<subtask> read_subtasks(int task_id) {
            if (task_id <= 0) 
                throw todo_file_exception("Not a valid id.");

            const char* query = R"(
                SELECT
                    subtask_id,
                    task_id,
                    subtask,
                    priority,
                    date_implemented,
                    due_date,
                    date_completed,
                    status
                FROM subtask
                WHERE task_id = ?;
            )";

            sqlite3_open(TODO_SUBTASKS, &db);
            sqlite3_stmt* rstmt = nullptr;

            if (sqlite3_prepare_v2(db, query, -1, &rstmt, nullptr) != SQLITE_OK) 
                throw todo_file_exception("Error during query preparation.");

            std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> stmt(rstmt, sqlite3_finalize);

            if (sqlite3_bind_int(stmt.get(), 1, task_id) != SQLITE_OK)
                throw todo_file_exception("Error binding task_id.");

            std::vector<subtask> returned = std::vector<subtask>();
            int result;

            while ((result = sqlite3_step(stmt.get())) == SQLITE_ROW) {
                subtask st = subtask{
                    sqlite3_column_int(stmt.get(), 0),
                    sqlite3_column_int(stmt.get(), 1),

                    get_text(stmt.get(), 2),
                    subtask::to_priority(get_text(stmt.get(), 3)),

                    get_text(stmt.get(), 4),
                    get_text(stmt.get(), 5),
                    get_text(stmt.get(), 6),

                    subtask::to_status(get_text(stmt.get(), 7))
                };

                returned.push_back(st);
            }

            if (result != SQLITE_DONE)
                throw todo_file_exception("Error reading subtasks.");

            return returned;
        }

        int remove_subtask(const int subtask_id) {
            if (subtask_id <= 0)
                throw todo_file_exception("Not a valid id.");

            sqlite3_open(TODO_SUBTASKS, &db);
            const char* query = R"(DELETE FROM subtask WHERE subtask_id = ?)";

            auto Subtask = this->read_subtask(subtask_id);

            sqlite3_stmt* rstmt = nullptr;

            if (sqlite3_prepare_v2(db, query, -1, &rstmt, nullptr) != SQLITE_OK) 
                throw todo_file_exception("Error during query preparation.");

            std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> stmt(rstmt, sqlite3_finalize);

            if (sqlite3_bind_int(stmt.get(), 1, subtask_id) != SQLITE_OK)
                throw todo_file_exception("Error binding subtask_id.");

            int result = sqlite3_step(stmt.get());

            if (result == SQLITE_DONE) {
                log_generator l = log_generator();

                dialog::report(report_types::info, "Subtask '%s' removed successfully.", Subtask.subtask.c_str());
                std::string holder = std::string("Subtask '");
                holder += Subtask.subtask + "' was removed from database.";

                const char* msg = l.log_message(holder);
                l.generate(msg);
            }

            return result;
        }

        vector<subtask> read_subtasks() {
            const char* query = R"(
                SELECT
                    subtask_id,
                    task_id,
                    subtask,
                    priority,
                    date_implemented,
                    due_date,
                    date_completed,
                    status
                FROM subtask
            )";

            sqlite3_open(TODO_SUBTASKS, &db);
            sqlite3_stmt* rstmt = nullptr;

            if (sqlite3_prepare_v2(db, query, -1, &rstmt, nullptr) != SQLITE_OK) 
                throw todo_file_exception("Error during query preparation.");

            std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> stmt(rstmt, sqlite3_finalize);

            std::vector<subtask> returned = std::vector<subtask>();
            int result;

            while ((result = sqlite3_step(stmt.get())) == SQLITE_ROW) {
                subtask st = subtask{
                    sqlite3_column_int(stmt.get(), 0),
                    sqlite3_column_int(stmt.get(), 1),

                    get_text(stmt.get(), 2),
                    subtask::to_priority(get_text(stmt.get(), 3)),

                    get_text(stmt.get(), 4),
                    get_text(stmt.get(), 5),
                    get_text(stmt.get(), 6),

                    subtask::to_status(get_text(stmt.get(), 7))
                };

                returned.push_back(st);
            }

            if (result != SQLITE_DONE)
                throw todo_file_exception("Error reading subtasks.");

            return returned;
        }
    };
}