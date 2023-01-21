#include <Architecture.h>

namespace Architecture {
    RegisterID reg_from_string(std::string reg) const {
        RegisterID* r;
        switch(reg) {
            case "rdi":
                *r = RegisterID::rdi;
                break;
            case "rax":
                *r = RegisterID::rax;
                break;
            case "rsi":
                *r = RegisterID::rsi;
                break;
            case "rdx":
                *r = RegisterID::rdx;
                break;
            case "rcx":
                *r = RegisterID::rcx;
                break;
            case "r8":
                *r = RegisterID::r8;
                break;
            case "r9":
                *r = RegisterID::r9;
                break;
            case "rbx":
                *r = RegisterID::rbx;
                break;
            case "rbp":
                *r = RegisterID::rbp;
                break;
            case "r10":
                *r = RegisterID::r10;
                break;
            case "r11":
                *r = RegisterID::r11;
                break;
            case "r12":
                *r = RegisterID::r12;
                break;
            case "r13":
                *r = RegisterID::r13;
                break;
            case "r14":
                *r = RegisterID::r14;
                break;
            case "r15":
                *r = RegisterID::r15;
                break;
            case "rsp":
                *r = RegisterID::rsp;
                break;
        }
        return *r;
    };
    CompareOP cmpOP_from_string(std::string cmp) const {
        CompareOP* ret_cmp;
        switch(cmp) {
            case "<":
                *ret_cmp = CompareOP::less_than;
                break;
            case "<=":
                *ret_cmp = CompareOP::less_than_or_equal;
                break;
            case "=":
                *ret_cmp = CompareOP::equal;
                break;
        }
        return *ret_cmp;
    };
    OP OP_from_string(std::string op) const {
        OP* ret_op;
        switch(op) {
            case "++":
                *ret_op = OP::plus_plus;
                break;
            case "--":
                *ret_op = OP::minus_minus;
                break;
            case "+=":
                *ret_op = OP::plus_equals;
                break;
            case "--":
                *ret_op = OP::minus_equals;
                break;
            case "*=":
                *ret_op = OP::multiply_equals;
                break;
            case "&=":
                *ret_op = OP::and_equals;
                break;
            case "@":
                *ret_op = OP::lea;
                break;
            case "<<=":
                *ret_op = OP::shift_left;
                break;
            case ">>=":
                *ret_op = OP::shift_right;
                break;
        }
    };
}