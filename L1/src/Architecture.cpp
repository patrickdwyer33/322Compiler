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

    std::unordered_map<RegisterID, std::string> string_from_reg_map = {
        {RegisterID::rdi, "rdi"},
        {RegisterID::rax, "rax"},
        {RegisterID::rsi, "rsi"},
        {RegisterID::rdx, "rdx"},
        {RegisterID::rcx, "rcx"},
        {RegisterID::r8, "r8"},
        {RegisterID::r9, "r9"},
        {RegisterID::rbx, "rbx"},
        {RegisterID::rbp, "rbp"},
        {RegisterID::r10, "r10"},
        {RegisterID::r11, "r11"},
        {RegisterID::r12, "r12"},
        {RegisterID::r13, "r13"},
        {RegisterID::r14, "r14"},
        {RegisterID::r15, "r15"},
        {RegisterID::rsp, "rsp"}
    };

    std::string to_string(Architecture::RegisterID r) {
        return string_from_reg_map[r];
    }

    std::unordered_map<CompareOP, std::string> string_from_cmpOP_map = {
        {CompareOP::less_than, "<"},
        {CompareOP::less_than_or_equal, "<="},
        {CompareOP::equal, "="}
    };

    std::string to_string(Architecture::CompareOP cmpOP) {
        return string_from_cmpOP_map[cmpOP];
    }

    std::unordered_map<OP, std::string> string_from_OP_map = {
        {OP::plus_plus, "++"},
        {OP::minus_minus, "--"},
        {OP::plus_equals, "+="},
        {OP::minus_equals, "-="},
        {OP::multiply_equals, "*="},
        {OP::and_equals, "&="},
        {OP::lea, "@"},
        {OP::shift_left, "<<="},
        {OP::shift_right, ">>="}
    };

    std::string to_string(Architecture::OP op) {
        return string_from_OP_map[op];
    }

    std::unordered_map<Architecture::RegisterID, std::string> instr_from_reg_map = {
        {RegisterID::rdi, "dil"},
        {RegisterID::rax, "al"},
        {RegisterID::rsi, "sil"},
        {RegisterID::rdx, "dl"},
        {RegisterID::rcx, "cl"},
        {RegisterID::r8, "r8b"},
        {RegisterID::r9, "r9b"},
        {RegisterID::rbx, "bl"},
        {RegisterID::rbp, "bpl"},
        {RegisterID::r10, "r10b"},
        {RegisterID::r11, "r11b"},
        {RegisterID::r12, "r12b"},
        {RegisterID::r13, "r13b"},
        {RegisterID::r14, "r14b"},
        {RegisterID::r15, "r15b"},
        {RegisterID::rsp, "rsp"}
    };

    std::string get_eight_bit(Architecture::RegisterID r) {
        return instr_from_reg_map[r];
    }

    std::unordered_map<CompareOP, std::string> instr_from_cmp_map = {
        {CompareOP::less_than, "l"},
        {CompareOP::less_than_or_equal, "le"},
        {CompareOP::equal, "e"}
    };

    std::string get_cmp_instr(Architecture::CompareOP cmpOP) {
        return instr_from_cmp_map[cmpOP];
    }

    std::unordered_map<OP, std::string> instr_from_op_map = {
        {OP::plus_plus, "inc"},
        {OP::minus_minus, "dec"},
        {OP::plus_equals, "addq"},
        {OP::minus_equals, "subq"},
        {OP::multiply_equals, "imulq"},
        {OP::and_equals, "andq"},
        {OP::lea, "lea"},
        {OP::shift_left, "salq"},
        {OP::shift_right, "sarq"}
    };

    std::pair<std::string, bool> get_op_instr(Architecture::OP op) {
        bool to_shift = false;
        if (op == OP::shift_left || op == OP::shift_right) {
            to_shift = true;
        }
        return std::make_pair(instr_from_op_map[op], to_shift);
    }

}