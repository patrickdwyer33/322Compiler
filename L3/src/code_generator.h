#include <L3.h>

namespace L3 {
    class Code_Generator_Visitor : L3::Visitor {
        void visit(Instruction_return* i);
        void visit(Instruction_label* i);
        void visit(Instruction_branch* i);
        void visit(Instruction_call* i);
        void visit(Instruction_assignment* i);
        void visit(Function* fn);
        void visit(Program* p);
    };
}