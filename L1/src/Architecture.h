#pragma once

#include <vector>
#include <string>

namespace Architecture {
    enum RegisterID {rdi, rax, rsi, rdx, rcx, r8, r9, rbx, rbp, r10, r11, r12, r13, r14, r15, rsp};
    enum CompareOP {less_than, equal, less_than_or_equal};
    enum OP {plus_plus, minus_minus, plus_equals, minus_equals, multiply_equals, and_equals, lea, shift_left, shift_right};

    RegisterID reg_from_string(std::string reg) const;
    CompareOP cmpOP_from_string(std::string cmp) const;
    OP OP_from_string(std::string op) const;

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