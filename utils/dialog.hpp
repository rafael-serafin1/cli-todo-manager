#pragma once

#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <windows.h>

#ifndef DIALOG_TODO_
#define DIALOG_TODO_

#define MODELO_ESC_RGB      "\033[38;2;R;G;Bm"

#define BOLD_ESC        "\x1b[1m"
#define FAINT_ESC       "\x1b[2m"
#define ITALIC_ESC      "\x1b[3m"
#define UNDERLINE_ESC   "\x1b[4m"

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
        purple,
        gray,
        grey,
        orange
    };
    
    enum class report_types {
        success,
        error,
        warn,
        info,
        debug,
        ignorable,
        log,
        unknown
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

            case dialog_types::gray:        return "\033[38;2;80;80;80m";
            case dialog_types::grey:        return "\033[38;2;180;180;180m";
            case dialog_types::orange:      return "\033[38;2;255;152;0m";
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

            case report_types::ignorable:
                return color(dialog_types::grey);

            case report_types::log:
                return color(dialog_types::white);
            }

            return color(dialog_types::black);
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

            case report_types::ignorable:
                return "[ ignorable ]";
            }

            return "[ unknown ]";
        }

    public:
        static void message(dialog_types _MessageType, const char* _MessageContent, ...) {
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

        static void report(report_types _MessageType, const char* _MessageContent, ...) {
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

        static std::string output(dialog_types _Type, const char* _Message) {
            const char* esc = color(_Type);   
            return std::string(esc) + _Message + "\x1b[0m";
        }
    };
}

#endif