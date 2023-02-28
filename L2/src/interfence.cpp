#include <interfence.h>
#include <Architecture.h>
#include <unordered_set>
#include <iostream>
#include <string>
#include <utility>

namespace L2 {

    L2::fence_visitor fence_builder;
    L2::fence_printer f_printer;

    std::unordered_set<L2::Variable, Variable::HashFunction> GP_registers;

    void initialize_GP_registers() {
        auto caller_save_registers = Architecture::get_caller_save_registers();
        auto callee_save_registers = Architecture::get_callee_save_registers();
        for (auto reg: caller_save_registers) {
            L2::Register* reg_var = new L2::Register(Architecture::to_string(reg), reg);
            GP_registers.insert(*reg_var);
        }
        for (auto reg: callee_save_registers) {
            L2::Register* reg_var = new L2::Register(Architecture::to_string(reg), reg);
            GP_registers.insert(*reg_var);
        }
        return;
    }

    void fence_visitor::visit(L2::Instruction_return* i) {

        return;
    }
    void fence_visitor::visit(L2::Instruction_assignment* i) {

        return;
    }
    void fence_visitor::visit(L2::Instruction_operation* i) {

        return;
    }
    void fence_visitor::visit(L2::Instruction_save_cmp* i) {

        return;
    }
    void fence_visitor::visit(L2::Instruction_cjump* i) {

        return;
    }
    void fence_visitor::visit(L2::Instruction_label* i) {

        return;
    }
    void fence_visitor::visit(L2::Instruction_goto* i) {

        return;
    }
    void fence_visitor::visit(L2::Instruction_call* i) {

        return;
    }
    void fence_visitor::visit(L2::Instruction_call_print* i) {

        return;
    }
    void fence_visitor::visit(L2::Instruction_call_input* i) {

        return;
    }
    void fence_visitor::visit(L2::Instruction_call_allocate* i) {

        return;
    }
    void fence_visitor::visit(L2::Instruction_call_tensorError* i) {
        
        return;
    }

    void fence_visitor::visit(L2::Program* p) {
        initialize_GP_registers();
        for (auto f: p->functions) {
            f->accept(&fence_builder);
        }
        return;
    }

    void fence_visitor::visit(L2::Function* fn) {

        L2::fence_graph* g = new L2::fence_graph();
        
        for (auto reg : GP_registers) {
            L2::fence_node* n = new L2::fence_node();
            n->var = &reg;
            for (auto reg2 : GP_registers) {
                if (reg == reg2) continue;
                n->neighbors.insert(reg2);
            }
            std::pair<std::string, L2::fence_node> node_entry = std::make_pair(reg.to_string(), *n);
            g->node_map.insert(node_entry);
        }

        for (int idx = 0; idx < fn->instructions.size(); idx++) {
            auto instr = fn->instructions[idx];
            for (auto var : fn->ins[idx]) {
                for (auto var2 : fn->ins[idx]) {
                    if (var == var2) continue;
                    if (g->node_map.find(var.to_string()) == g->node_map.end()) {
                        L2::fence_node* n = new L2::fence_node();
                        n->var = &var;
                        std::pair<std::string, L2::fence_node> node_entry = std::make_pair(var.to_string(), *n);
                        g->node_map.insert(node_entry);
                    }
                    if (g->node_map.find(var2.to_string()) == g->node_map.end()) {
                        L2::fence_node* n = new L2::fence_node();
                        n->var = &var2;
                        std::pair<std::string, L2::fence_node> node_entry = std::make_pair(var2.to_string(), *n);
                        g->node_map.insert(node_entry);
                    }
                    g->node_map[var.to_string()].neighbors.insert(var2);
                    g->node_map[var2.to_string()].neighbors.insert(var);
                }
            }
            for (auto var : fn->outs[idx]) {
                for (auto var2 : fn->outs[idx]) {
                    if (var == var2) continue;
                    if (g->node_map.find(var.to_string()) == g->node_map.end()) {
                        L2::fence_node* n = new L2::fence_node();
                        n->var = &var;
                        std::pair<std::string, L2::fence_node> node_entry = std::make_pair(var.to_string(), *n);
                        g->node_map.insert(node_entry);
                    }
                    if (g->node_map.find(var2.to_string()) == g->node_map.end()) {
                        L2::fence_node* n = new L2::fence_node();
                        n->var = &var2;
                        std::pair<std::string, L2::fence_node> node_entry = std::make_pair(var2.to_string(), *n);
                        g->node_map.insert(node_entry);
                    }
                    g->node_map[var.to_string()].neighbors.insert(var2);
                    g->node_map[var2.to_string()].neighbors.insert(var);
                }
            }
            for (auto var : instr->kill) {
                for (auto var2 : fn->outs[idx]) {
                    if (var == var2) continue;
                    if (g->node_map.find(var.to_string()) == g->node_map.end()) {
                        L2::fence_node* n = new L2::fence_node();
                        n->var = &var;
                        std::pair<std::string, L2::fence_node> node_entry = std::make_pair(var.to_string(), *n);
                        g->node_map.insert(node_entry);
                    }
                    if (g->node_map.find(var2.to_string()) == g->node_map.end()) {
                        L2::fence_node* n = new L2::fence_node();
                        n->var = &var2;
                        std::pair<std::string, L2::fence_node> node_entry = std::make_pair(var2.to_string(), *n);
                        g->node_map.insert(node_entry);
                    }
                    g->node_map[var.to_string()].neighbors.insert(var2);
                    g->node_map[var2.to_string()].neighbors.insert(var);
                }
            }
            L2::Instruction_operation* op_instr = dynamic_cast<L2::Instruction_operation*>(instr);
            if (op_instr != NULL) {
                std::vector<Item*> items = op_instr->get();
                L2::Operation* op = static_cast<L2::Operation*>(items[1]);
                if (op->get() == Architecture::OP::shift_left || op->get() == Architecture::OP::shift_right) {
                    L2::Variable* var = dynamic_cast<L2::Variable*>(items[2]);
                    if (var != NULL) {
                        if (g->node_map.find(var->to_string()) == g->node_map.end()) {
                            L2::fence_node* n = new L2::fence_node();
                            n->var = var;
                            std::pair<std::string, L2::fence_node> node_entry = std::make_pair(var->to_string(), *n);
                            g->node_map.insert(node_entry);
                        }
                        for (auto reg : GP_registers) {
                            if (reg.to_string() == "rcx") continue;
                            if (reg == *var) continue;
                            g->node_map[var->to_string()].neighbors.insert(reg);
                            g->node_map[reg.to_string()].neighbors.insert(*var);
                        }
                    }
                }
            }
        }
        fn->interfence_graph = g;
        return;
    }

    // ***
    // PRINTER BELOW
    // ***

    void fence_printer::visit(L2::Instruction_return* i) {

        return;
    }
    void fence_printer::visit(L2::Instruction_assignment* i) {

        return;
    }
    void fence_printer::visit(L2::Instruction_operation* i) {
    
        return;
    }
    void fence_printer::visit(L2::Instruction_save_cmp* i) {
    
        return;
    }
    void fence_printer::visit(L2::Instruction_cjump* i) {

        return;
    }
    void fence_printer::visit(L2::Instruction_label* i) {

        return;
    }
    void fence_printer::visit(L2::Instruction_goto* i) {

        return;
    }
    void fence_printer::visit(L2::Instruction_call* i) {

        return;
    }
    void fence_printer::visit(L2::Instruction_call_print* i) {

        return;
    }
    void fence_printer::visit(L2::Instruction_call_input* i) {

        return;
    }
    void fence_printer::visit(L2::Instruction_call_allocate* i) {

        return;
    }
    void fence_printer::visit(L2::Instruction_call_tensorError* i) {

        return;
    }

    void fence_printer::visit(L2::Program* p) {
        for (auto f: p->functions) {
            f->accept(&f_printer);
        }
        return;
    }

    void fence_printer::visit(L2::Function* fn) {
        for (auto& x : fn->interfence_graph->node_map) {
            std::cout << x.first;
            for (auto var : x.second.neighbors) {
                std::cout << " " << var.to_string();
            }
            std::cout << "\n";
        }
        return;
    }

    void generate_fence(L2::Program &p) {
        p.accept(&fence_builder);
        return;
    }
    void print_fence(L2::Program &p) {
        p.accept(&f_printer);
        return;
    }

}