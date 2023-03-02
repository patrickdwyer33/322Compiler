#pragma once

#include <L2.h>

namespace L2 {

    void spill(L2::Program &p);
    void spill_all(L2::Function* fn);
    void print_spill(L2::Program &p);

    class spiller : public L2::Visitor {
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