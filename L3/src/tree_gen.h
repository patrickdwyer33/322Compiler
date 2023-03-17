#include <L3.h>

namespace L3 {

    void Generate_Trees(Program &p);
    
    class Tree_Visitor : public L3::Visitor {
        void visit(Instruction_return* i);
        void visit(Instruction_label* i);
        void visit(Instruction_branch* i);
        void visit(Instruction_call* i);
        void visit(Instruction_assignment* i);
        void visit(Function* fn);
        void visit(Program* p);
    };
}