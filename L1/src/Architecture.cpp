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
    }

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

    std::unordered_map<std::string, OP> string_from_OP_map = {
        {OP::plus_plus, "++"},
        {OP::minus_minus, "--"},
        {OP::plus_equals, "+="},
        {OP::minus_equals, "--"},
        {OP::minus_equals, "-="},
        {OP::multiply_equals, "*="},
        {OP::and_equals, "&="},
        {OP::lea, "@"},
        {OP::shift_left, "<<="},
        {OP::shift_right, ">>="}
    };

    std::string to_string(Architecture::OP cmpOP) {
        return string_from_OP_map[op];
    }

    std::unordered_map<Architecture::RegisterID r, std::string> instr_from_reg_map = {
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
    }

    std::string get_eight_bit(Architecture::RegisterID r) {
        
    }

    std::pair<std::string, bool> get_op_instr(CompareOP raw_op) {
        std::string instruction;
        bool to_shift = false;
        switch (raw_op) {
            case Architecture::OP::plus_equals:
                instruction = "addq";
                break;
            case Architecture::OP::minus_equals:
                instruction = "subq";
                break;
            case Architecture::OP::multiply_equals:
                instruction = "imulq";
                break;
            case Architecture::OP::and_equals:
                instruction = "andq";
                break;
            case Architecture::OP::shift_left:
                instruction = "salq";
                to_shift = true;
                break;
            case Architecture::OP::shift_right:
                instruction = "sarq";
                to_shift = true;
                break;
            default:
                std::sterr << "ERROR in generate_op r1 <- r2" << std::endl;
        }
        return std::make_pair(instruction, to_shift);
    }
}