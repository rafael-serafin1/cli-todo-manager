#pragma once

#ifndef SUBTASK_SQL_H
#define SUBTASK_SQL_H

#include <vector>
#include <string>
#include <cctype>
#include <memory>

#include "../../../include/sqlite3.h"
#include "../../../file/file.hpp"
#include "../../../utils/dialog.hpp"
#include "subtasks.hpp"

#ifdef _WIN32
    #define TODO_SUBTASKS_SQL       ".todo\\subtasks.db"
#else
    #define TODO_SUBTASKS_SQL       ".todo/subtasks.db"
#endif

using namespace TODO;
using namespace std;

namespace TODO {
    class subtask_sql_handler: public subtask_handler {
    private:
        sqlite3* db = nullptr;

        void open_database() {
            if (db)
                return;

            if (sqlite3_open(TODO_SUBTASKS_SQL, &db) != SQLITE_OK) {
                const std::string error = (db) ? sqlite3_errmsg(db) 
                    : "Unable to allocate database connection.";

                if (db)
                    sqlite3_close(db);
                    
                db = nullptr;
                throw todo_file_exception("Error opening subtask database: " + error);
            }
        }

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
        subtask_sql_handler() { }

        ~subtask_sql_handler() {
            if (db)
                sqlite3_close(db);
        }

        void create_database() override {
            open_database();
        }

        void create_table() override {
            char* error_message = nullptr;
        
            const auto* query = R"(
                CREATE TABLE IF NOT EXISTS subtask (
                    subtask_id INTEGER PRIMARY KEY AUTOINCREMENT,
                    task_id    INTEGER NOT NULL,
                    subtask    TEXT NOT NULL,
                    priority   INTEGER NOT NULL DEFAULT 0,
                    
                    date_implemented DATE NOT NULL DEFAULT (DATE('now')),
                    due_date         DATE NULL,
                    date_completed   DATE NULL,
                    status           INTEGER NOT NULL DEFAULT 0,

                    CONSTRAINT ck_subtask_priority 
                        CHECK (priority IN (0, 1, 2, 3, 4)),

                    CONSTRAINT ck_subtask_task_id 
                        CHECK (task_id > 0),

                    CONSTRAINT ck_subtask_status 
                        CHECK (status IN (0, 1, 2, 3, 4))
                );
            )";
            
            int result = sqlite3_exec(db, query, nullptr, nullptr, &error_message);

            if (result != SQLITE_OK) {
                sqlite3_free(error_message);  
                throw todo_file_exception("Error during subtask table creation.");
            }

            dialog::report(report_types::debug, "Table created with success!"); 
        }

        int insert_subtask(subtask subtask) override {
            if (subtask.task_id <= 0)
                throw todo_file_exception("Subtasks can't have an ID below or equals 0.");
            
            open_database();
            const char* query = R"(
                INSERT INTO subtask(task_id, subtask, priority, due_date, status) VALUES 
                (?, ?, ?, ?, ?);
            )";

            sqlite3_stmt* rstmt = nullptr;

            if (sqlite3_prepare_v2(db, query, -1, &rstmt, nullptr) != SQLITE_OK)
                throw todo_file_exception("Error during query preparation.");
            
            auto stmt = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>(rstmt, sqlite3_finalize);

            if (sqlite3_bind_int(stmt.get(), 1, subtask.task_id) != SQLITE_OK)
                throw todo_file_exception("Error binding task_id.");

            if (sqlite3_bind_text(stmt.get(), 2, subtask.subtask.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
                throw todo_file_exception("Error binding subtask.");

            if (sqlite3_bind_int(stmt.get(), 3, (int) subtask.priority) != SQLITE_OK)
                throw todo_file_exception("Error binding priority.");

            bind_date(stmt.get(), 4, subtask.due_date);

            if (sqlite3_bind_int(stmt.get(), 5, (int) subtask.status) != SQLITE_OK)
                throw todo_file_exception("Error binding status.");
                
            if (sqlite3_step(stmt.get()) != SQLITE_DONE)
                throw todo_file_exception("Error inserting subtask: " + std::string(sqlite3_errmsg(db)));

            dialog::report(report_types::info, "Subtask '%s' added successfully.", subtask.subtask.c_str());

            log_generator l = log_generator();
            auto msg = l.log_message("Subtask '" + subtask.subtask + "' was removed from database.");
            l.generate(msg);

            return sqlite3_last_insert_rowid(db);
        }

        subtask read_subtask(int subtask_id) override {
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

            open_database();
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

                (subtask::subtask_priority) sqlite3_column_int(stmt.get(), 3),

                get_text(stmt.get(), 4),

                get_text(stmt.get(), 5),

                get_text(stmt.get(), 6),

                (subtask::subtask_status) sqlite3_column_int(stmt.get(), 7),
            };
        }

        vector<subtask> read_subtasks(int task_id) override {
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

            open_database();
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
                    (subtask::subtask_priority) sqlite3_column_int(stmt.get(), 3),

                    get_text(stmt.get(), 4),
                    get_text(stmt.get(), 5),
                    get_text(stmt.get(), 6),

                    (subtask::subtask_status) sqlite3_column_int(stmt.get(), 7),
                };

                returned.push_back(st);
            }

            if (result != SQLITE_DONE)
                throw todo_file_exception("Error reading subtasks.");

            return returned;
        }

        int remove_subtask(const int subtask_id) override {
            if (subtask_id <= 0)
                throw todo_file_exception("Not a valid id.");

            open_database();
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

                const std::string msg = l.log_message(holder);
                l.generate(msg);
            }

            return result;
        }

        vector<subtask> read_subtasks() override {
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

            open_database();
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
                    (subtask::subtask_priority) sqlite3_column_int(stmt.get(), 3),

                    get_text(stmt.get(), 4),
                    get_text(stmt.get(), 5),
                    get_text(stmt.get(), 6),

                    (subtask::subtask_status) sqlite3_column_int(stmt.get(), 7)
                };

                returned.push_back(st);
            }

            if (result != SQLITE_DONE)
                throw todo_file_exception("Error reading subtasks.");

            return returned;
        }

        subtask edit_subtask(const int subtask_id, subtask now) override {
            if (now.task_id <= 0)
                throw todo_file_exception("Subtasks can't have an ID below or equals 0.");

            subtask old = read_subtask(subtask_id);
            now.id = subtask_id;

            const char* query = R"(
                UPDATE subtask
                SET task_id             = ?,
                    subtask             = ?,
                    priority            = ?,
                    due_date            = ?,
                    status              = ?
                WHERE subtask_id = ?
            )";

            sqlite3_stmt* rstmt = nullptr;

            if (sqlite3_prepare_v2(db, query, -1, &rstmt, nullptr) != SQLITE_OK)
                throw todo_file_exception("Error during query preparation.");

            std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> stmt(rstmt, sqlite3_finalize);

            if (sqlite3_bind_int(stmt.get(), 1, now.task_id) != SQLITE_OK)
                throw todo_file_exception("Error binding 'task_id'.");

            if (sqlite3_bind_text(stmt.get(), 2, now.subtask.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
                throw todo_file_exception("Error binding 'subtask'.");

            if (sqlite3_bind_int(stmt.get(), 3, (int) now.priority) != SQLITE_OK)
                throw todo_file_exception("Error binding 'priority'.");
            bind_date(stmt.get(), 4, now.due_date);

            if (sqlite3_bind_int(stmt.get(), 5, (int) now.status) != SQLITE_OK)
                throw todo_file_exception("Error binding 'status'.");

            if (sqlite3_bind_int(stmt.get(), 6, subtask_id) != SQLITE_OK)
                throw todo_file_exception("Error binding 'subtask_id'.");

            if (sqlite3_step(stmt.get()) != SQLITE_DONE)
                throw todo_file_exception("Error editing subtask: " + std::string(sqlite3_errmsg(db)));

            dialog::report(report_types::info, "Subtask '%s' edited successfully.", now.subtask.c_str());

            log_generator l = log_generator();
            auto msg = 
                l.log_message("Subtask '" + std::to_string(subtask_id) + "' was edited, changing from '" + old.subtask + "' to '" + now.subtask + "'.");
            l.generate(msg);

            return now;
        }
    };
}

#endif