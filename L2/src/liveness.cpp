#include <unordered_set>
#include <unordered_map>
#include <string>
#include <liveness.h>
#include <Architecture.h>
#include <iostream>

namespace L2 {

    L2::Liveness_Analyzer analyzer;
    L2::Liveness_Printer printer;

    std::unordered_set<L2::Variable*> caller_save_vars;

    void initialize_caller_save_vars() {
        caller_save_vars.clear();
        auto caller_save_registers = Architecture::get_caller_save_registers();
        for (auto reg: caller_save_registers) {
            L2::Register* reg_var = new L2::Register(Architecture::to_string(reg), reg);
            caller_save_vars.insert(reg_var);
        }
    }

    std::unordered_set<L2::Variable*> callee_save_vars;

    void initialize_callee_save_vars() {
        callee_save_vars.clear();
        auto callee_save_registers = Architecture::get_callee_save_registers();
        for (auto reg: callee_save_registers) {
            L2::Register* reg_var = new L2::Register(Architecture::to_string(reg), reg);
            callee_save_vars.insert(reg_var);
        }
    }

    std::unordered_map<int, Architecture::RegisterID> first_6_vars_map = {
        {0, Architecture::RegisterID::rdi},
        {1, Architecture::RegisterID::rsi},
        {2, Architecture::RegisterID::rdx},
        {3, Architecture::RegisterID::rcx},
        {4, Architecture::RegisterID::r8},
        {5, Architecture::RegisterID::r9}
    };

    void Liveness_Analyzer::visit(L2::Instruction_return* i) {
        i->succs.clear();
        i->kill.clear();
        i->gen = callee_save_vars;
        L2::Register* rax = new L2::Register(Architecture::to_string(Architecture::RegisterID::rax), Architecture::RegisterID::rax);
        i->gen.insert(rax);
        return;
    }
    void Liveness_Analyzer::visit(L2::Instruction_assignment* i) {
        i->succs.clear();
        int succ = i->idx + 1;
        i->succs.insert(succ);
        i->gen.clear();
        i->kill.clear();
        std::vector<L2::Item*> items = i->get();
        L2::Variable* v1 = dynamic_cast<L2::Variable*>(items[0]);
        if (v1 != NULL) {
            i->kill.insert(v1);
        } else {
            L2::MemoryLocation* mem = dynamic_cast<L2::MemoryLocation*>(items[0]);
            if (mem != NULL) {
                L2::Variable* x = static_cast<L2::Variable*>(mem->get()[0]);
                if (x->get() != "rsp") {
                    i->gen.insert(x);
                }
            }
        }
        L2::Variable* v2 = dynamic_cast<L2::Variable*>(items[1]);
        if (v2 != NULL) {
            i->gen.insert(v2);
        } else {
            L2::MemoryLocation* mem = dynamic_cast<L2::MemoryLocation*>(items[1]);
            if (mem != NULL) {
                L2::Variable* x = static_cast<L2::Variable*>(mem->get()[0]);
                if (x->get() != "rsp") {
                    i->gen.insert(x);
                }
            }
        }
        return;
    }
    void Liveness_Analyzer::visit(L2::Instruction_operation* i) {
        i->succs.clear();
        i->gen.clear();
        i->kill.clear();
        int succ = i->idx + 1;
        i->succs.insert(succ);
        std::vector<L2::Item*> items = i->get();
        L2::Variable* dst = dynamic_cast<L2::Variable*>(items[0]);
        L2::NullItem* second = dynamic_cast<L2::NullItem*>(items[2]);
        L2::NullItem* third = dynamic_cast<L2::NullItem*>(items[3]);
        if (dst != NULL && second != NULL) {
            i->kill.insert(dst);
            i->gen.insert(dst);
            return;
        } else if (dst == NULL) {
            L2::MemoryLocation* mem = static_cast<L2::MemoryLocation*>(items[0]);
            dst = static_cast<L2::Variable*>(mem->get()[0]);
            if (dst->get() != "rsp") {
                i->gen.insert(dst);
            }
        } else if (third == NULL) {
            i->kill.insert(dst);
        } else {
            i->kill.insert(dst);
            i->gen.insert(dst);
        }
        L2::Variable* v1 = dynamic_cast<L2::Variable*>(items[2]);
        if (v1 != NULL) {
            i->gen.insert(v1);
        } else {
            L2::MemoryLocation* mem = dynamic_cast<L2::MemoryLocation*>(items[2]);
            if (mem != NULL) {
                L2::Variable* x = static_cast<L2::Variable*>(mem->get()[0]);
                if (x->get() != "rsp") {
                    i->gen.insert(x);
                }
                return;
            }
        }
        L2::Variable* v2 = dynamic_cast<L2::Variable*>(items[3]);
        if (v2 != NULL) {
            i->gen.insert(v2);
        }
        return;
    }
    void Liveness_Analyzer::visit(L2::Instruction_save_cmp* i) {
        i->succs.clear();
        i->gen.clear();
        i->kill.clear();
        int succ = i->idx + 1;
        i->succs.insert(succ);
        std::vector<L2::Item*> items = i->get();
        L2::Variable* dst = static_cast<L2::Variable*>(items[0]);
        i->kill.insert(dst);
        L2::Variable* v1 = dynamic_cast<L2::Variable*>(items[1]);
        if (v1 != NULL) {
            i->gen.insert(v1);
        }
        L2::Variable* v2 = dynamic_cast<L2::Variable*>(items[3]);
        if (v2 != NULL) {
            i->gen.insert(v2);
        }
        return;
    }
    void Liveness_Analyzer::visit(L2::Instruction_cjump* i) {
        i->succs.clear();
        i->kill.clear();
        i->gen.clear();
        std::vector<L2::Item*> items = i->get();
        L2::Variable* v1 = dynamic_cast<L2::Variable*>(items[0]);
        if (v1 != NULL) {
            i->gen.insert(v1);
        }
        L2::Variable* v2 = dynamic_cast<L2::Variable*>(items[2]);
        if (v2 != NULL) {
            i->gen.insert(v2);
        }
        int succ = i->idx + 1;
        i->succs.insert(succ);
        //succ2 is added in fnction visitor
        return;
    }
    void Liveness_Analyzer::visit(L2::Instruction_label* i) {
        i->succs.clear();
        int succ = i->idx + 1;
        i->succs.insert(succ);
        i->gen.clear();
        i->kill.clear();
        return;
    }
    void Liveness_Analyzer::visit(L2::Instruction_goto* i) {
        i->succs.clear();
        i->gen.clear();
        i->kill.clear();
        return;
    }
    void Liveness_Analyzer::visit(L2::Instruction_call* i) {
        i->succs.clear();
        int succ = i->idx + 1;
        i->succs.insert(succ);
        std::vector<Item*> items = i->get();
        L2::Number* num_args = static_cast<L2::Number*>(items[1]);
        int64_t real_n = num_args->get();
        int n = static_cast<int>(real_n);
        i->gen.clear();
        for (int idx = 0; idx < n; idx++) {
            if (idx < 6) {
                Architecture::RegisterID reg = first_6_vars_map[idx];
                L2::Register* reg_var = new L2::Register(Architecture::to_string(reg), reg);
                i->gen.insert(reg_var);
            } else {
                break;
            }
        }
        i->kill = caller_save_vars;
        L2::Variable* fn_called = dynamic_cast<L2::Variable*>(items[0]);
        if (fn_called != NULL) {
            i->gen.insert(fn_called);
        }
        return;
    }
    void Liveness_Analyzer::visit(L2::Instruction_call_print* i) {
        i->succs.clear();
        int succ = i->idx + 1;
        i->succs.insert(succ);
        i->kill = caller_save_vars;
        i->gen.clear();
        L2::Register* rdi_var = new L2::Register(Architecture::to_string(Architecture::RegisterID::rdi), Architecture::RegisterID::rdi);
        i->gen.insert(rdi_var);
        return;
    }
    void Liveness_Analyzer::visit(L2::Instruction_call_input* i) {
        i->succs.clear();
        int succ = i->idx + 1;
        i->succs.insert(succ);
        i->kill = caller_save_vars;
        i->gen.clear();
        return;
    }
    void Liveness_Analyzer::visit(L2::Instruction_call_allocate* i) {
        i->succs.clear();
        int succ = i->idx + 1;
        i->succs.insert(succ);
        i->kill = caller_save_vars;
        L2::Register* rdi_var = new L2::Register(Architecture::to_string(Architecture::RegisterID::rdi), Architecture::RegisterID::rdi);
        i->gen.insert(rdi_var);
        L2::Register* rsi_var = new L2::Register(Architecture::to_string(Architecture::RegisterID::rsi), Architecture::RegisterID::rsi);
        i->gen.insert(rsi_var);
        return;
    }
    void Liveness_Analyzer::visit(L2::Instruction_call_tensorError* i) {
        i->succs.clear();
        i->kill = caller_save_vars;
        i->gen.clear();
        L2::Item* item_arg = i->get();
        L2::Number* num_arg = static_cast<L2::Number*>(item_arg);
        int64_t real_n = num_arg->get();
        int n = static_cast<int>(real_n);
        for (int idx = 0; idx < n; idx++) {
            if (idx < 6) {
                Architecture::RegisterID reg = first_6_vars_map[idx];
                L2::Register* reg_var = new L2::Register(Architecture::to_string(reg), reg);
                i->gen.insert(reg_var);
            } else {
                break;
            }
        }
        return;
    }

    void Liveness_Analyzer::visit(L2::Program* p) {
        for (auto f: p->functions) {
            f->accept(&analyzer);
        }
        return;
    }

    void Liveness_Analyzer::visit(L2::Function* fn) {
        initialize_callee_save_vars();
        initialize_caller_save_vars();
        for (int i = 0; i < fn->instructions.size(); i++) {
            fn->instructions[i]->idx = i;
            fn->instructions[i]->accept(&analyzer);
            L2::Instruction_cjump* cjump_instr = dynamic_cast<L2::Instruction_cjump*>(fn->instructions[i]);
            if (cjump_instr != NULL) {
                std::vector<L2::Item*> items = cjump_instr->get();
                L2::Label* label = static_cast<L2::Label*>(items[3]);
                fn->instructions[i]->succs.insert(fn->label_map[label->get()]);
            } else {
                L2::Instruction_goto* goto_instr = dynamic_cast<L2::Instruction_goto*>(fn->instructions[i]);
                if (goto_instr != NULL) {
                    L2::Label* label = static_cast<L2::Label*>(goto_instr->get());
                    fn->instructions[i]->succs.insert(fn->label_map[label->get()]);
                }
            }
        }
        fn->ins.clear();
        fn->outs.clear();
        for (int i = 0; i < fn->instructions.size(); i++) {
            std::unordered_set<L2::Variable*> in;
            fn->ins.push_back(in);
            std::unordered_set<L2::Variable*> out;
            fn->outs.push_back(out);
        }
        bool changed = true;
        while (changed) {
            changed = false;
            for (int i = fn->instructions.size() - 1; i >= 0; i--) {
                std::unordered_set<L2::Variable*> new_in = fn->instructions[i]->gen;
                std::unordered_set<L2::Variable*> new_out;
                for (auto idx: fn->instructions[i]->succs) {
                    for (auto &v: fn->ins[idx]) {
                        new_out.insert(v);
                    }
                }
                for (auto &v: new_out) {
                    if (fn->instructions[i]->kill.find(v) == fn->instructions[i]->kill.end()) {
                        new_in.insert(v);
                    }
                }
                
                if (fn->ins[i] != new_in || fn->outs[i] != new_out) {
                    changed = true;
                }
                fn->ins[i] = new_in;
                fn->outs[i] = new_out;

            }
        }
        
        return;
    }

    // ***
    // PRINTER BELOW
    // ***

    void Liveness_Printer::visit(L2::Instruction_return* i) {

        return;
    }
    void Liveness_Printer::visit(L2::Instruction_assignment* i) {

        return;
    }
    void Liveness_Printer::visit(L2::Instruction_operation* i) {
    
        return;
    }
    void Liveness_Printer::visit(L2::Instruction_save_cmp* i) {
    
        return;
    }
    void Liveness_Printer::visit(L2::Instruction_cjump* i) {

        return;
    }
    void Liveness_Printer::visit(L2::Instruction_label* i) {

        return;
    }
    void Liveness_Printer::visit(L2::Instruction_goto* i) {

        return;
    }
    void Liveness_Printer::visit(L2::Instruction_call* i) {

        return;
    }
    void Liveness_Printer::visit(L2::Instruction_call_print* i) {

        return;
    }
    void Liveness_Printer::visit(L2::Instruction_call_input* i) {

        return;
    }
    void Liveness_Printer::visit(L2::Instruction_call_allocate* i) {

        return;
    }
    void Liveness_Printer::visit(L2::Instruction_call_tensorError* i) {

        return;
    }

    void Liveness_Printer::visit(L2::Program* p) {
        for (auto f: p->functions) {
            f->accept(&printer);
        }
        return;
    }

    void Liveness_Printer::visit(L2::Function* fn) {
        std::cout << "(\n(in\n";
        std::string out_string = ")\n\n(out\n";
        for (int i = 0; i < fn->instructions.size(); i++) {
            std::cout << "(";
            for (auto v: fn->ins[i]) {
                std::cout << v->to_string() << " ";
            }
            std::cout<< ")\n";
            out_string.append("(");
            for (auto v: fn->outs[i]) {
                out_string.append(v->to_string());
                out_string.append(" ");
            }
            out_string.append(")\n");
        }
        out_string.append(")\n\n)\n");
        std::cout << out_string;
        return;
    }

    void generate_liveness(L2::Program &p) {
        p.accept(&analyzer);
        return;
    }

    void generate_liveness_fn(L2::Function* fn) {
        fn->accept(&analyzer);
        return;
    }

    void print_liveness(L2::Program &p) {
        p.accept(&printer);
        return;
    }
}
