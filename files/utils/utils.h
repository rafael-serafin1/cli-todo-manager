#include <string.h>

int is_checked(const char *_str) {
    for (int i = 0; i < strlen(_str); ++i) {
        if (_str[i] == '[') {
            if (_str[i + 1] == 'X')
                return 1;
            else
                return 0;
        }
    }

    return 0;
}

int is_unchecked(const char *_str) {
    for (int i = 0; i < strlen(_str); ++i) {
        if (_str[i] == '[') {
            if (_str[i + 1] == ' ')
                return 1;
            else
                return 0;
        }
    }

    return 0;
}

int is_ongoing(const char* _str) {
    for (int i = 0; i < strlen(_str); ++i) {
        if (_str[i] == '[') {
            if (_str[i + 1] == '-')
                return 1;
            else
                return 0;
        }
    }

    return 0;
}