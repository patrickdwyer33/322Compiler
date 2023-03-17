#include <code_generator.h>
#include <fstream>
#include <string>

namespace L3 {

    std::ofstream outputFile;

    Code_Generator_Visitor code_gen_vis;

    std::vector<std::string> arg_regs;

    // make sure this is the right order
    void instatiate_arg_regs() {
        arg_regs.clear();
        arg_regs.push_back("rdi");
        arg_regs.push_back("rsi");
        arg_regs.push_back("rdx");
        arg_regs.push_back("rcx");
        arg_regs.push_back("r8");
        arg_regs.push_back("r9");
    }

    void Code_Generator_Visitor::visit(Instruction_return* i) {
        if (i->returns_val) {
            outputFile << i->return_val->s << "\n";
        }
        outputFile << "return\n";
        return;
    }
    void Code_Generator_Visitor::visit(Instruction_label* i) {
        outputFile << i->s << "\n";
        return;
    }
    void Code_Generator_Visitor::visit(Instruction_branch* i) {
        outputFile << i->s << "\n";
        return;
    }
    void Code_Generator_Visitor::visit(Instruction_call* i) {
        outputFile << "mem rsp -8 <- :" << i->fn_name << "_ret\n";
        if (i->num_args > 6) {
            for (int idx = 7; idx <= i->num_args; idx++) {
                int offset = idx - 6;
                outputFile << "mem rsp -" << std::to_string(offset) << " <- " + i->args[idx-1]->s << "\n";
            }
        }
        for (int idx = 0; idx < 6; idx++) {
            std::string cur_reg = arg_regs[idx];
            outputFile << cur_reg << " <- " << i->args[idx]->s << "\n";
        }
        outputFile << "call @" << i->fn_name << std::to_string(i->num_args) << "\n";
        outputFile << i->fn_name << "_ret\n";
        return;

    }
    void Code_Generator_Visitor::visit(Instruction_assignment* i) {
        if (i->is_L2) {
            outputFile << i->s << "\n";
            return;
        }
        i->call_instr->accept(&code_gen_vis);
        outputFile << "rax <- " << i->left->s << "\n";
        return;
    }
    void Code_Generator_Visitor::visit(Function* fn) {
        outputFile << "(@" << fn->name << "\n" << std::to_string(fn->num_args) << "\n";
        for (uint64_t idx = fn->num_args; idx > 0; --idx) {
            std::string cur_reg = arg_regs[idx-1];
            outputFile << fn->vars[idx-1]->s << " <- " << cur_reg << "\n";
        }
        for (auto instr : fn->instructions) {
            if (instr->is_L2) {
                outputFile << instr->s << "\n";
            }
            else {
                instr->accept(&code_gen_vis);
            }
        }
        outputFile << ")\n";
    }
    void Code_Generator_Visitor::visit(Program* p) {
        outputFile << "(@main\n";
        for (auto fn : p->functions) {
            fn->accept(&code_gen_vis);
        }
        outputFile << ")\n";
    }

    void Generate_L2(Program &p) {
        outputFile.open("prog.L2");
        p.accept(&code_gen_vis);
        outputFile.close();
    }
}