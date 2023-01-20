#pragma once

#include <vector>
#include <string>

namespace Architecture {
    enum RegisterID {rdi, rax, rsi, rdx, rcx, r8, r9, rbx, rbp, r10, r11, r12, r13, r14, r15, rsp};
    enum CompareOP {less_than, equal, less_than_or_equal};
    enum OP {plus_plus, minus_minus, plus_equals, minus_equals, multiply_equals, and_equals, ampersand, shift_left, shift_right};
    /*
    class Item {
        public:
            virtual std::string to_string() const = 0;
    };

    class Register : public Item {
        public:
            virtual std::string to_string() const;
    };

    class Label : public Item {
        public:
            virtual std::string to_string() const;
    }
    */
}