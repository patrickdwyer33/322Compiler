#include <interfence.h>
#include <Architecture.h>
#include <unordered_set>
#include <iostream>
#include <string>
#include <utility>

namespace L2 {

    L2::fence_visitor fence_builder;
    L2::fence_printer f_printer;

    std::unordered_set<L2::Variable*> GP_registers;

    void initialize_GP_registers() {
        auto caller_save_registers = Architecture::get_caller_save_registers();
        auto callee_save_registers = Architecture::get_callee_save_registers();
        for (auto reg: caller_save_registers) {
            L2::Register* reg_var = new L2::Register(Architecture::to_string(reg), reg);
            GP_registers.insert(reg_var);
        }
        for (auto reg: callee_save_registers) {
            L2::Register* reg_var = new L2::Register(Architecture::to_string(reg), reg);
            GP_registers.insert(reg_var);
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
        for (auto f: p->functions) {
            f->accept(&fence_builder);
        }
        return;
    }

    void add_fence_nodes(std::unordered_map<std::string, L2::fence_node> &map, L2::Variable* var, L2::Variable* var2) 
    {
        bool found_first = false;
        bool found_second = false;
        if (map.find(var->get()) != map.end()) {
            found_first = true;
            L2::fence_node node = map[var->get()];
            if (node.neighbors.find(var2) == node.neighbors.end()) {
                node.neighbors.insert(var2);
                node.num_neighbors += 1;
            }
        }
        if (map.find(var2->get()) != map.end()) {
            found_second = true;
            L2::fence_node node = map[var2->get()];
            if (node.neighbors.find(var) == node.neighbors.end()) {
                node.neighbors.insert(var);
                node.num_neighbors += 1;
            }
        }
        if (found_first && found_second) {
            return;
        }
        else if (found_second) {
            // add first
            L2::fence_node node = new L2::fence_node(var);
            map[var->get()] = node;
            if (node.neighbors.find(var2) == node.neighbors.end()) {
                node.neighbors.insert(var2);
                node.num_neighbors += 1;
            }
        }
        else if (found_first) {
            // add second
            L2::fence_node node = new L2::fence_node(var2);
            map[var2->get()] = node;
            if (node.neighbors.find(var) == node.neighbors.end()) {
            node.neighbors.insert(var);
            nod.>num_neighbors += 1;
            }
        }
        else {
            // add first
            L2::fence_node node = new L2::fence_node(var);
            map[var->get()] = node;
            if (node.neighbors.find(var2) == node.neighbors.end()) {
                node.neighbors.insert(var2);
                node.num_neighbors += 1;
            }
            // add second
            L2::fence_node* node2 = new L2::fence_node(var2);
            map[var2->get()] = node2;
            if (node2.neighbors.find(var) == node2.neighbors.end()) {
            node2.neighbors.insert(var);
            node2.num_neighbors += 1;
            }
        }
        
        
        return;
    }

    void fence_visitor::visit(L2::Function* fn) {

        L2::fence_graph* g = new L2::fence_graph();
        
        for (auto reg : GP_registers) {
            for (auto reg2 : GP_registers) {
                if (*reg == *reg2) continue;
                add_fence_nodes(g->node_map, reg, reg2);
            }
        }

        for (int idx = 0; idx < fn->instructions.size(); idx++) {
            auto instr = fn->instructions[idx];
            for (auto var : fn->ins[idx]) {
                for (auto var2 : fn->ins[idx]) {
                    if (*var == *var2) continue;
                    add_fence_nodes(g->node_map, var, var2);
                }
            }
            for (auto var : fn->outs[idx]) {
                for (auto var2 : fn->outs[idx]) {
                    if (*var == *var2) continue;
                    add_fence_nodes(g->node_map, var, var2);
                }
            }
            for (auto var : instr->kill) {
                for (auto var2 : fn->outs[idx]) {
                    if (*var == *var2) continue;
                    add_fence_nodes(g->node_map, var, var2);
                }
            }
            L2::Instruction_operation* op_instr = dynamic_cast<L2::Instruction_operation*>(instr);
            if (op_instr != NULL) {
                std::vector<Item*> items = op_instr->get();
                L2::Operation* op = static_cast<L2::Operation*>(items[1]);
                if (op->get() == Architecture::OP::shift_left || op->get() == Architecture::OP::shift_right) {
                    L2::Variable* var = dynamic_cast<L2::Variable*>(items[2]);
                    if (var != NULL) {
                        for (auto reg : GP_registers) {
                            if (reg->get() == "rcx") continue;
                            if (*reg == *var) continue;
                            add_fence_nodes(g->node_map, var, reg);
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
        for (auto x : fn->interfence_graph->node_map) {
            std::cout << x.first;
            for (auto &var : x.second->neighbors) {
                std::cout << " " << var->get();
            }
            std::cout << "\n";
        }
        return;
    }

    void generate_fence(L2::Program &p) {
        initialize_GP_registers();
        p.accept(&fence_builder);
        return;
    }

    void generate_fence_fn(L2::Function* fn) {
        initialize_GP_registers();
        fn->accept(&fence_builder);
        return;
    }

    void print_fence(L2::Program &p) {
        p.accept(&f_printer);
        return;
    }

}