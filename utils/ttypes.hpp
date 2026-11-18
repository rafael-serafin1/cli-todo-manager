#pragma once

#include <iostream>

namespace TODO {
    typedef class Natural {
        private:
            std::size_t raw;
            std::size_t pos;

        public:
            Natural(long long int num)
                : raw(num), pos(num) {}

            Natural(std::size_t num)
                : raw(num), pos(num) {}

            std::size_t raw_value() const {
                return raw;
            }

            std::size_t position() const {
                return pos;
            }

            friend bool operator<(std::size_t left, const Natural& right) {
                return left < right.pos;
            }

            friend bool operator<=(std::size_t left, const Natural& right) {
                return left <= right.pos;
            }

            friend bool operator>(std::size_t left, const Natural& right) {
                return left > right.pos;
            }

            friend bool operator>=(std::size_t left, const Natural& right) {
                return left >= right.pos;
            }

            friend bool operator==(std::size_t left, const Natural& right) {
                return left == right.pos;
            }

            friend bool operator!=(std::size_t left, const Natural& right) {
                return left != right.pos;
            }
    } nat;
}