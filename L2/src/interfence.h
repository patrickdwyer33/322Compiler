#pragma once

#include <L2.h>

namespace L2 {

    void generate_fence(L2::Program &p);
    void print_fence(L2::Program &p);

    class fence_visitor : public L2::Visitor {
        public:
            void visit(Instruction_return* i);
            void visit(Instruction_assignment* i);
            void visit(Instruction_operation* i);
            void visit(Instruction_cjump* i);
            void visit(Instruction_save_cmp* i);
            void visit(Instruction_label* i);
            void visit(Instruction_goto* i);
            void visit(Instruction_call* i);
            void visit(Instruction_call_print* i);
            void visit(Instruction_call_input* i);
            void visit(Instruction_call_allocate* i);
            void visit(Instruction_call_tensorError* i);
            void visit(Function* fn);
            void visit(Program* p);
    };

    class fence_printer : public L2::Visitor {
        public:
            void visit(Instruction_return* i);
            void visit(Instruction_assignment* i);
            void visit(Instruction_operation* i);
            void visit(Instruction_cjump* i);
            void visit(Instruction_save_cmp* i);
            void visit(Instruction_label* i);
            void visit(Instruction_goto* i);
            void visit(Instruction_call* i);
            void visit(Instruction_call_print* i);
            void visit(Instruction_call_input* i);
            void visit(Instruction_call_allocate* i);
            void visit(Instruction_call_tensorError* i);
            void visit(Function* fn);
            void visit(Program* p);
    };

}