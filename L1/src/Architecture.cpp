#include <Architecture.h>
#include <unordered_map>

namespace Architecture {

    std::unordered_map<std::string, RegisterID> reg_from_string_map = {
        {"rdi", RegisterID::rdi},
        {"rax", RegisterID::rax},
        {"rsi", RegisterID::rsi},
        {"rdx", RegisterID::rdx},
        {"rcx", RegisterID::rcx},
        {"r8", RegisterID::r8},
        {"r9", RegisterID::r9},
        {"rbx", RegisterID::rbx},
        {"rbp", RegisterID::rbp},
        {"r10", RegisterID::r10},
        {"r11", RegisterID::r11},
        {"r12", RegisterID::r12},
        {"r13", RegisterID::r13},
        {"r14", RegisterID::r14},
        {"r15", RegisterID::r15},
        {"rsp", RegisterID::rsp}
    };

    RegisterID reg_from_string(std::string r) {
        return reg_from_string_map[r];
    } 

    std::unordered_map<std::string, CompareOP> cmpOP_from_string_map = {
        {"<", CompareOP::less_than},
        {"<=", CompareOP::less_than_or_equal},
        {"=", CompareOP::equal}
    };

    CompareOP cmpOP_from_string(std::string cmpOP) {
        return cmpOP_from_string_map[cmpOP];
    } 

    std::unordered_map<std::string, OP> OP_from_string_map = {
        {"++", OP::plus_plus},
        {"--", OP::minus_minus},
        {"+=", OP::plus_equals},
        {"--", OP::minus_equals},
        {"-=", OP::minus_equals},
        {"*=", OP::multiply_equals},
        {"&=", OP::and_equals},
        {"@", OP::lea},
        {"<<=", OP::shift_left},
        {">>=", OP::shift_right}
    };

    OP OP_from_string(std::string op) {
        return OP_from_string_map[op];
    } 

    std::string to_string(Architecture::RegisterID r) {
        std::string s;
        switch(r) {
            case RegisterID::rdi:
                s = "rdi";
                break;
            case RegisterID::rax:
                s = "rax";
                break;
            case RegisterID::rsi:
                s = "rsi";
                break;
            case RegisterID::rdx:
                s = "rdx";
                break;
            case RegisterID::rcx:
                s = "rcx";
                break;
            case RegisterID::r8:
                s = "r8";
                break;
            case RegisterID::r9:
                s = "r9";
                break;
            case RegisterID::rbx:
                s = "rbx";
                break;
            case RegisterID::rbp:
                s = "rbp";
                break;
            case RegisterID::r10:
                s = "r10";
                break;
            case RegisterID::r11:
                s = "r11";
                break;
            case RegisterID::r12:
                s = "r12";
                break;
            case RegisterID::r13:
                s = "r13";
                break;
            case RegisterID::r14:
                s = "r14";
                break;
            case RegisterID::r15:
                s = "r15";
                break;
            case RegisterID::rsp:
                s = "rsp";
                break;
        }
        return s;
    }
}