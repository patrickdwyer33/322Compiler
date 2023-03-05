#pragma once

#include <L2.h>
#include <Architecture.h>

namespace L2 {

    void generate_liveness(L2::Program &p);
    void generate_liveness_fn(L2::Function* fn);
    void print_liveness(L2::Program &p);

    class Liveness_Analyzer : public L2::Visitor {
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

    class Liveness_Printer : public L2::Visitor {
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