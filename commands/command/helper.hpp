#pragma once

#include <iostream>

namespace TODO {
    struct TodoTask {
        bool checked;
        std::string title;
    };

    enum class priority {
        NONE    = 0,
        LOW     = 1,
        MEDIUM  = 2,
        HIGH    = 3,
        URGENT  = 4
    };

    const std::string stringfy(priority p) {
        switch (p) {
            case priority::NONE:            return "none";
            case priority::LOW:             return "low";
            case priority::MEDIUM:          return "medium";
            case priority::HIGH:            return "high";
            case priority::URGENT:          return "urgent";
            default:                        throw todo_file_exception("Unknown type of priority.");
        }
    }

    priority to_priority(const std::string& str) {
        std::string p = "";

        for (const auto& i: str) 
            p += std::toupper(i);
        
        if (p == "NONE")        return priority::NONE;
        if (p == "LOW")         return priority::LOW;
        if (p == "MEDIUM")      return priority::MEDIUM;
        if (p == "HIGH")        return priority::HIGH;
        if (p == "URGENT")      return priority::URGENT;

        throw todo_file_exception("Unknow type of priority.");
    }

    std::string format_output(const std::string& message, priority priority) {
        std::string output; 

        switch (priority) {
            case priority::NONE:                output += message;                                                      
                break;
            case priority::LOW:                 output += std::string(FAINT_ESC) + message;                             
                break;
            case priority::MEDIUM:              output += std::string(ITALIC_ESC) + message;                            
                break;
            case priority::HIGH:                output += std::string(ITALIC_ESC) + std::string(BOLD_ESC) + message;    
                break;
            case priority::URGENT:              output += std::string(ITALIC_ESC) + std::string(BOLD_ESC) + std::string(UNDERLINE_ESC) +  message;
                break;
            default:                                    throw todo_file_exception("Unexpected status.");
        }

        output += "\x1b[0m";
        return output;
    }
}