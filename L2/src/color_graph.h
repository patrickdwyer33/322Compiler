#pragma once

#include <L2.h>
#include <Architecture.h>
#include <vector>

namespace L2 {

    void initiate_color_order();

    void sort_nodes(L2::Function* fn);

    void remove_node(L2::Function* fn);

    bool bigger_node(L2::fence_node left, L2::fence_node right);

    void color_function(L2::Function* fn);

    void color_program(L2::Program &p);

    class colorer_visitor : public L2::Visitor {
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