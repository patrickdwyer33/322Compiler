#pragma once

#include <string>
#include <unordered_set>

namespace Architecture {
    enum RegisterID {rdi, rax, rsi, rdx, rcx, r8, r9, rbx, rbp, r10, r11, r12, r13, r14, r15, rsp};
    enum CompareOP {less_than, equal, less_than_or_equal, greater_than, greater_than_or_equal};
    enum OP {plus_plus, minus_minus, plus_equals, minus_equals, multiply_equals, and_equals, lea, shift_left, shift_right};

    std::unordered_set<RegisterID> get_caller_save_registers();
    std::unordered_set<RegisterID> get_callee_save_registers();

    RegisterID reg_from_string(std::string r);
    CompareOP cmpOP_from_string(std::string cmpOP);
    OP OP_from_string(std::string op);

    std::string to_string(RegisterID r);
    std::string to_string(CompareOP cmpOP);
    std::string to_string(OP op);

    std::string get_eight_bit(RegisterID r);
    std::string get_cmp_instr(CompareOP cmpOP);
    std::pair<std::string, bool> get_op_instr(OP op);
}