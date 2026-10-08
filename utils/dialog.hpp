#pragma once

#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <windows.h>

#ifndef DIALOG_TODO_
#define DIALOG_TODO_

namespace TODO {
    enum class dialog_types {
        red,
        green,
        blue,
        yellow,
        white,
        black,
        dark_green,
        dark_red,
        dark_yellow,
        dark_blue,
        cyan,
        magenta,
        purple
    };
    enum class report_types {
        success,
        error,
        warn,
        info,
        debug,
        log
    };

    class dialog {
    private:
        static const char* color(dialog_types type) {
            switch (type) {
            case dialog_types::red:         return "\033[91m";
            case dialog_types::green:       return "\033[92m";
            case dialog_types::blue:        return "\033[94m";
            case dialog_types::yellow:      return "\033[93m";
            case dialog_types::white:       return "\033[97m";
            case dialog_types::black:       return "\033[30m";

            case dialog_types::dark_green:  return "\033[32m";
            case dialog_types::dark_red:    return "\033[31m";
            case dialog_types::dark_yellow: return "\033[33m";
            case dialog_types::dark_blue:   return "\033[34m";

            case dialog_types::cyan:        return "\033[96m";
            case dialog_types::magenta:     return "\033[95m";
            case dialog_types::purple:      return "\033[35m";
            }

            return "\033[0m";
        }

        static const char* report_color(report_types type) {
            switch (type) {
            case report_types::success:
                return color(dialog_types::green);

            case report_types::error:
                return color(dialog_types::red);

            case report_types::warn:
                return color(dialog_types::yellow);

            case report_types::info:
                return color(dialog_types::cyan);

            case report_types::debug:
                return color(dialog_types::magenta);

            case report_types::log:
                return color(dialog_types::white);
            }

            return color(dialog_types::white);
        }

        static const char* report_prefix(report_types type) {
            switch (type) {
            case report_types::success:
                return "[ success ]";

            case report_types::error:
                return "[ error ]";

            case report_types::warn:
                return "[ warn ]";

            case report_types::info:
                return "[ info ]";

            case report_types::debug:
                return "[ debug ]";

            case report_types::log:
                return "[ log ]";
            }

            return "[ unknown ]";
        }

    public:
        static void message(dialog_types _MessageType,const char* _MessageContent, ...) {
            va_list args;
            va_start(args, _MessageContent);

            char buffer[1024];
            vsnprintf(buffer, sizeof(buffer), _MessageContent, args);

            va_end(args);

            // Cor da mensagem
            std::cout << color(_MessageType)
                    << buffer
                    << "\033[0m"
                    << '\n';
        }

        static void report(report_types _MessageType,const char* _MessageContent, ...) {
            va_list args;
            va_start(args, _MessageContent);

            char buffer[1024];

            vsnprintf(
                buffer,
                sizeof(buffer),
                _MessageContent,
                args
            );

            va_end(args);

            const char* reportColor = report_color(_MessageType);
            const char* prefix = report_prefix(_MessageType);

            std::cout
                << reportColor
                << prefix
                << "\033[0m "
                << buffer
                << '\n';
        }
    };
}

#endif