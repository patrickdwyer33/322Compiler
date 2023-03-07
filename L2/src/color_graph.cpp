#include <color_graph.h>
#include <algorithm>
#include <spiller.h>
#include <liveness.h>
#include <interfence.h>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <queue>
#include <cassert>

namespace L2 {

    L2::colorer_visitor colorer;
    
    uint64_t num_colors = 15;

    L2::Function* cur_fn;

    void initiate_color_order(L2::Function* fn) {
        auto caller_save_regs = Architecture::get_caller_save_registers();
        auto callee_save_regs = Architecture::get_callee_save_registers();
        fn->sorted_colors.clear();
        fn->color_map.clear();
        uint64_t index = 0;
        for (auto reg : caller_save_regs) {
            fn->sorted_colors.push_back(reg);
            fn->color_map[Architecture::to_string(reg)] = index;
            index++;
        }
        for (auto reg : callee_save_regs) {
            fn->sorted_colors.push_back(reg);
            fn->color_map[Architecture::to_string(reg)] = index;
            index++;
        }
        return;
    }

    bool fence_cmp (const L2::fence_node &left, const L2::fence_node &right) {
        uint64_t left_size = left.neighbors.size();
        uint64_t right_size = right.neighbors.size();
        if (left_size >= num_colors && right_size >= num_colors) {
          return left_size < right_size;
        }
        else if (right_size >= num_colors) {
          return false;
        }
        else if (left_size >= num_colors) {
            return true;
        }
        else {
          return left_size < right_size;
        }
      }

    void sort_nodes(std::vector<L2::fence_node> &sorted_fence_nodes, const std::unordered_map<std::string, L2::fence_node> &node_map) {
        if (!sorted_fence_nodes.empty()) {
            sorted_fence_nodes.clear();
        }
        for (auto it : node_map) {
            sorted_fence_nodes.push_back(it.second);
        }
        std::sort(sorted_fence_nodes.begin(), sorted_fence_nodes.end(), fence_cmp);
        return;
    }

    void remove_node(std::vector<L2::fence_node> &sorted_fence_nodes, std::unordered_map<std::string, L2::fence_node> &node_map) {
        if (sorted_fence_nodes.empty()) {
            return;
        }
        L2::fence_node to_remove = sorted_fence_nodes.back();
        for (auto var : to_remove.neighbors) {
            assert(node_map.find(var.get()) != node_map.end());
            node_map[var.get()].neighbors.erase(to_remove.var_str);
            node_map[var.get()].num_neighbors -= 1;
        }
        sorted_fence_nodes.pop_back();
        return;
    }

    void fix_data(L2::Function* fn) {
        for (auto &it : fn->interfence_graph->node_map) {
            it.second.var_str = it.first;
        }
    }

    void color_function(L2::Function* fn) {
        fix_data(fn);
        initiate_color_order(fn);
        bool cant_color = false;
        std::vector<L2::fence_node> sorted_fence_nodes;
        std::unordered_map<std::string, L2::fence_node> old_map;
        old_map = fn->interfence_graph->node_map;
        sort_nodes(sorted_fence_nodes, old_map);
        uint64_t num_nodes = sorted_fence_nodes.size();
        for (uint64_t i = 0; i < num_nodes; i++) {
            std::vector<uint64_t> neighbor_colors;
            L2::fence_node cur_fence_node = sorted_fence_nodes.back();
            if (fn->color_map.find(cur_fence_node.var_str) != fn->color_map.end()) {
                remove_node(sorted_fence_nodes, fn->interfence_graph->node_map);
                continue;
            }
            std::unordered_map<uint64_t, bool> seen_map;
            for (auto var : old_map[cur_fence_node.var_str].neighbors) {
                if (fn->color_map.find(var.get()) != fn->color_map.end() && seen_map.find(fn->color_map[var.get()]) == seen_map.end()) {
                    neighbor_colors.push_back(fn->color_map[var.get()]);
                    seen_map[fn->color_map[var.get()]] = true;
                }
            }
            std::sort(neighbor_colors.begin(), neighbor_colors.end());
            for (uint64_t j = 0; j < num_colors; j++) {
                if (j >= neighbor_colors.size()) {
                    fn->color_map[cur_fence_node.var_str] = j;
                    break;
                }
                if (neighbor_colors[j] == j) continue;
                else {
                    fn->color_map[cur_fence_node.var_str] = j;
                    break;
                }
            }
            if (fn->color_map.find(cur_fence_node.var_str) == fn->color_map.end()) cant_color = true;
            remove_node(sorted_fence_nodes, fn->interfence_graph->node_map);
        }
        if (cant_color) {
            fn->interfence_graph->node_map = old_map;
            std::cout << "be spillin\n";
            L2::spill_all(fn);
            generate_liveness_fn(fn);
            generate_fence_fn(fn);
            color_function(fn);
        }
        return;
    }

    void colorer_visitor::visit(L2::Instruction_return* i) {

        return;
    }
    void colorer_visitor::visit(L2::Instruction_assignment* i) {
        auto items = i->get();
        L2::Variable* var1 = dynamic_cast<L2::Variable*>(items[0]);
        if (var1 != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var1);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = cur_fn->sorted_colors[cur_fn->color_map[var1->to_string()]];
                L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
                i->dst = reg;
            }
        } else {
            L2::MemoryLocation* mem = dynamic_cast<L2::MemoryLocation*>(items[0]);
            if (mem != NULL) {
                L2::Variable* var = static_cast<L2::Variable*>(mem->get()[0]);
                L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var);
                if (reg_tstr == NULL) {
                    Architecture::RegisterID reg_id = cur_fn->sorted_colors[cur_fn->color_map[var->to_string()]];
                    L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
                    mem->base_var = reg;
                    i->dst = mem;
                }
            }
        }
        L2::Variable* var2 = dynamic_cast<L2::Variable*>(items[1]);
        if (var2 != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var2);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = cur_fn->sorted_colors[cur_fn->color_map[var2->to_string()]];
                L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
                i->src = reg;
            }
        } else {
            L2::MemoryLocation* mem = dynamic_cast<L2::MemoryLocation*>(items[1]);
            if (mem != NULL) {
                L2::Variable* var = static_cast<L2::Variable*>(mem->get()[0]);
                L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var);
                if (reg_tstr == NULL) {
                    Architecture::RegisterID reg_id = cur_fn->sorted_colors[cur_fn->color_map[var->to_string()]];
                    L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
                    mem->base_var = reg;
                    i->src = mem;
                }
            }
        }
        return;
    }
    void colorer_visitor::visit(L2::Instruction_operation* i) {
        auto items = i->get();
        L2::Variable* var1 = dynamic_cast<L2::Variable*>(items[0]);
        if (var1 != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var1);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = cur_fn->sorted_colors[cur_fn->color_map[var1->to_string()]];
            L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
            i->left = reg;
            }
            
        } else {
            L2::MemoryLocation* mem = dynamic_cast<L2::MemoryLocation*>(items[0]);
            if (mem != NULL) {
                L2::Variable* var = static_cast<L2::Variable*>(mem->get()[0]);
                L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var);
                if (reg_tstr == NULL) {
                    Architecture::RegisterID reg_id = cur_fn->sorted_colors[cur_fn->color_map[var->to_string()]];
                    L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
                    mem->base_var = reg;
                    i->left = mem;
                }
            }
        }
        L2::Variable* var2 = dynamic_cast<L2::Variable*>(items[2]);
        if (var2 != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var2);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = cur_fn->sorted_colors[cur_fn->color_map[var2->to_string()]];
            L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
            i->right = reg;
            }
            
        } else {
            L2::MemoryLocation* mem = dynamic_cast<L2::MemoryLocation*>(items[2]);
            if (mem != NULL) {
                L2::Variable* var = static_cast<L2::Variable*>(mem->get()[0]);
                L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var);
                if (reg_tstr == NULL) {
                    Architecture::RegisterID reg_id = cur_fn->sorted_colors[cur_fn->color_map[var->to_string()]];
                    L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
                    mem->base_var = reg;
                    i->right = mem;
                }
            }
        }
        L2::Variable* var3 = dynamic_cast<L2::Variable*>(items[3]);
        if (var3 != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var3);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = cur_fn->sorted_colors[cur_fn->color_map[var3->to_string()]];
            L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
            i->lea_reg = reg;
            }
            
        }
        return;
    }
    void colorer_visitor::visit(L2::Instruction_save_cmp* i) {
        auto items = i->get();
        L2::Variable* var1 = dynamic_cast<L2::Variable*>(items[0]);
        if (var1 != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var1);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = cur_fn->sorted_colors[cur_fn->color_map[var1->to_string()]];
                L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
                i->dst = reg;
            }
            
        }
        L2::Variable* var2 = dynamic_cast<L2::Variable*>(items[1]);
        if (var2 != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var2);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = cur_fn->sorted_colors[cur_fn->color_map[var2->to_string()]];
                L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
                i->left = reg;
            }
            
        }
        L2::Variable* var3 = dynamic_cast<L2::Variable*>(items[3]);
        if (var3 != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var3);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = cur_fn->sorted_colors[cur_fn->color_map[var3->to_string()]];
                L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
                i->right = reg;
            }
            
        }
        return;
    }
    void colorer_visitor::visit(L2::Instruction_cjump* i) {
        auto items = i->get();
        L2::Variable* var1 = dynamic_cast<L2::Variable*>(items[0]);
        if (var1 != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var1);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = cur_fn->sorted_colors[cur_fn->color_map[var1->to_string()]];
            L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
            i->left = reg;
            }
            
        }
        L2::Variable* var2 = dynamic_cast<L2::Variable*>(items[2]);
        if (var2 != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var2);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = cur_fn->sorted_colors[cur_fn->color_map[var2->to_string()]];
            L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
            i->right = reg;
            }
            
        }
        return;
    }
    void colorer_visitor::visit(L2::Instruction_label* i) {

        return;
    }
    void colorer_visitor::visit(L2::Instruction_goto* i) {

        return;
    }
    void colorer_visitor::visit(L2::Instruction_call* i) {
        auto items = i->get();
        L2::Variable* var = dynamic_cast<L2::Variable*>(items[0]);
        if (var != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = cur_fn->sorted_colors[cur_fn->color_map[var->to_string()]];
            L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
            i->fn = reg;
            }
            
        }
        return;
    }
    void colorer_visitor::visit(L2::Instruction_call_print* i) {

        return;
    }
    void colorer_visitor::visit(L2::Instruction_call_input* i) {

        return;
    }
    void colorer_visitor::visit(L2::Instruction_call_allocate* i) {

        return;
    }
    void colorer_visitor::visit(L2::Instruction_call_tensorError* i) {

        return;
    }

    void colorer_visitor::visit(L2::Program* p) {
        for (auto fn : p->functions) {
            fn->accept(&colorer);
        }
        return;
    }

    void colorer_visitor::visit(L2::Function* fn) {
        cur_fn = fn;
        for (auto instr : fn->instructions) {
            instr->accept(&colorer);
        }
        return;
    }

    void color_program(L2::Program &p) {
        for (auto fn : p.functions) {
            color_function(fn);
        }
        p.accept(&colorer);
        return;
    }

}