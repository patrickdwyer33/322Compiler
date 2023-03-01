#include <L2.h>

namespace L2 {

    std::string generate_code(L2::Program &p);
    void print_function(L2::Function &fn);

    class code_generator_visitor : public L2::Visitor {
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