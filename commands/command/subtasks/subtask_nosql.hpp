#pragma once

#ifndef SUBTASK_NOSQL_H
#define SUBTASK_NOSQL_H

#include "subtasks.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <string>
#include <stdexcept>
#include <vector>

#ifdef _WIN32
    #define TODO_SUBTASKS_NOSQL     ".todo\\subtasks.json"
    #include <windows.h>
#else
    #define TODO_SUBTASKS_NOSQL     ".todo/subtasks.json"
#endif


namespace TODO {
    class subtask_nosql_handler: public subtask_handler {
    private:
        enum class operation {
            read_all = 0,
            insert = 1,
            update = 2,
            remove = 3
        };

        std::wstring python_command =
            L"py -3 \"commands\\command\\subtasks\\subtasks.py\"";

        std::vector<subtask> subtask_parse(const std::string& payload) {
            std::vector<subtask> result;
            std::size_t offset = 0;

            const auto parse_integer = [](const std::string& value) {
                int parsed = 0;
                const auto conversion = std::from_chars(
                    value.data(), value.data() + value.size(), parsed
                );
                if (value.empty() || conversion.ec != std::errc{} ||
                    conversion.ptr != value.data() + value.size()) {
                    throw std::runtime_error("Invalid integer in subtask payload.");
                }
                return parsed;
            };

            while (offset < payload.size()) {
                std::array<std::string, 8> fields;

                for (std::string& field : fields) {
                    const std::size_t separator = payload.find('\n', offset);
                    if (separator == std::string::npos || separator == offset)
                        throw std::runtime_error("Invalid field length in subtask payload.");

                    std::size_t length = 0;
                    const auto conversion = std::from_chars(
                        payload.data() + offset,
                        payload.data() + separator,
                        length
                    );
                    if (conversion.ec != std::errc{} ||
                        conversion.ptr != payload.data() + separator) {
                        throw std::runtime_error("Invalid field length in subtask payload.");
                    }

                    offset = separator + 1;
                    if (length > payload.size() - offset)
                        throw std::runtime_error("Truncated field in subtask payload.");

                    field.assign(payload, offset, length);
                    offset += length;
                }

                result.push_back(subtask{
                    parse_integer(fields[0]),
                    parse_integer(fields[1]),
                    fields[2],
                    subtask::to_priority(fields[3].c_str()),
                    fields[4],
                    fields[5],
                    fields[6],
                    subtask::to_status(fields[7].c_str())
                });
            }

            return result;
        }

        std::vector<subtask> execute(operation action, const subtask& payload) {
            SECURITY_ATTRIBUTES sa{};
            sa.nLength = sizeof(sa);
            sa.bInheritHandle = TRUE;

            HANDLE child_stdin_read = nullptr;
            HANDLE parent_stdin_write = nullptr;
            HANDLE parent_stdout_read = nullptr;
            HANDLE child_stdout_write = nullptr;

            if (!CreatePipe(&child_stdin_read, &parent_stdin_write, &sa, 0))
                throw std::runtime_error("Failed to create Python stdin pipe.");

            if (!CreatePipe(&parent_stdout_read, &child_stdout_write, &sa, 0)) {
                CloseHandle(child_stdin_read);
                CloseHandle(parent_stdin_write);
                throw std::runtime_error("Failed to create Python stdout pipe.");
            }

            SetHandleInformation(parent_stdin_write, HANDLE_FLAG_INHERIT, 0);
            SetHandleInformation(parent_stdout_read, HANDLE_FLAG_INHERIT, 0);

            STARTUPINFOW startup{};
            startup.cb = sizeof(startup);
            startup.dwFlags = STARTF_USESTDHANDLES;
            startup.hStdInput = child_stdin_read;
            startup.hStdOutput = child_stdout_write;
            startup.hStdError = child_stdout_write;

            PROCESS_INFORMATION process{};
            std::wstring command = python_command + L" " +
                std::to_wstring(static_cast<int>(action));
                
            const BOOL created = CreateProcessW(
                nullptr, command.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW,
                nullptr, nullptr, &startup, &process
            );

            CloseHandle(child_stdin_read);
            CloseHandle(child_stdout_write);

            if (!created) {
                const DWORD error = GetLastError();
                CloseHandle(parent_stdin_write);
                CloseHandle(parent_stdout_read);
                throw std::runtime_error(
                    "Failed to start Python. Error: " + std::to_string(error)
                );
            }

            const std::string input = payload.dump();
            std::size_t offset = 0;
            bool write_ok = true;

            while (offset < input.size()) {
                DWORD written = 0;
                const DWORD chunk = static_cast<DWORD>(
                    std::min<std::size_t>(input.size() - offset, 65536)
                );

                if (!WriteFile(parent_stdin_write, input.data() + offset, chunk, &written, nullptr)
                    || written == 0) {
                    write_ok = false;
                    break;
                }
                offset += written;
            }
            CloseHandle(parent_stdin_write);

            std::string output;
            char buffer[4096];
            DWORD bytes_read = 0;
            for (;;) {
                if (!ReadFile(parent_stdout_read, buffer, sizeof(buffer), &bytes_read, nullptr) ||
                    bytes_read == 0) {
                    break;
                }
                output.append(buffer, bytes_read);
            }
            CloseHandle(parent_stdout_read);

            const DWORD wait_result = WaitForSingleObject(process.hProcess, INFINITE);
            DWORD exit_code = 0;
            const BOOL got_exit_code = GetExitCodeProcess(process.hProcess, &exit_code);
            CloseHandle(process.hThread);
            CloseHandle(process.hProcess);

            if (!write_ok)
                throw std::runtime_error("Failed to send the subtask to Python.");
            if (wait_result != WAIT_OBJECT_0 || !got_exit_code)
                throw std::runtime_error("Failed to wait for Python to finish.");
            if (exit_code != 0)
                throw std::runtime_error("Python failed: " + output);

            return subtask_parse(output);
        }

    public:
        void create_database() override {
            (void) execute(operation::read_all, subtask{});
        }

        void create_table() override { }

        int insert_subtask(subtask value) override {
            if (value.task_id <= 0)
                throw todo_file_exception("Subtasks can't have an ID below or equals 0.");

            const auto inserted = execute(operation::insert, value);
            
            if (inserted.size() != 1)
                throw std::runtime_error("Python did not return the inserted subtask.");
            return inserted.front().id;
        }

        int remove_subtask(const int subtask_id) override {
            if (subtask_id <= 0)
                throw todo_file_exception("Not a valid id.");

            subtask value{};
            value.id = subtask_id;
            const auto removed = execute(operation::remove, value);

            if (removed.size() != 1)
                throw std::runtime_error("Python did not return the removed subtask.");
            return 1;
        }

        subtask edit_subtask(const int subtask_id, subtask value) override {
            if (subtask_id <= 0 || value.task_id <= 0)
                throw todo_file_exception("Not a valid subtask or task ID.");

            value.id = subtask_id;
            const auto updated = execute(operation::update, value);

            if (updated.size() != 1)
                throw std::runtime_error("Python did not return the updated subtask.");
            return updated.front();
        }

        subtask read_subtask(int subtask_id) override {
            if (subtask_id <= 0)
                throw todo_file_exception("Not a valid id.");

            const auto subtasks = read_subtasks();
            const auto found = std::find_if(
                subtasks.begin(), subtasks.end(),
                [subtask_id](const subtask& value) { return value.id == subtask_id; }
            );

            if (found == subtasks.end())
                throw todo_file_exception("Subtask not found.");
            return *found;
        }

        std::vector<subtask> read_subtasks(int task_id) override {
            if (task_id <= 0)
                throw todo_file_exception("Not a valid id.");

            auto subtasks = read_subtasks();
            subtasks.erase(
                std::remove_if(
                    subtasks.begin(), subtasks.end(),
                    [task_id](const subtask& value) { return value.task_id != task_id; }
                ),
                subtasks.end()
            );
            return subtasks;
        }

        std::vector<subtask> read_subtasks() override {
            return execute(operation::read_all, subtask{});
        }
    };
}

#endif