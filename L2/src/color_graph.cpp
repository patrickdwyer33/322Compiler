#include <color_graph.h>
#include <algorithm>
#include <spiller.h>
#include <liveness.h>
#include <interfence.h>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <queue>

namespace L2 {

    L2::colorer_visitor colorer;

    std::vector<Architecture::RegisterID> sorted_colors;
    std::unordered_map<std::string, uint64_t> color_map;
    uint64_t num_colors = 15;

    L2::fence_graph fence_graph_copy;

    void initiate_color_order() {
        auto caller_save_regs = Architecture::get_caller_save_registers();
        auto callee_save_regs = Architecture::get_callee_save_registers();
        sorted_colors.clear();
        color_map.clear();
        uint64_t index = 0;
        for (auto reg : caller_save_regs) {
            sorted_colors.push_back(reg);
            color_map[Architecture::to_string(reg)] = index;
            index++;
        }
        for (auto reg : callee_save_regs) {
            sorted_colors.push_back(reg);
            color_map[Architecture::to_string(reg)] = index;
            index++;
        }
        return;
    }

    bool fence_cmp (const L2::fence_node* left, const L2::fence_node* right) {
        uint64_t left_size = left->num_neighbors;
        uint64_t right_size = right->num_neighbors;
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

    void sort_nodes(L2::Function* fn) {
        if (!fn->interfence_graph->sorted_fence_nodes.empty()) {
            fn->interfence_graph->sorted_fence_nodes.clear();
        }
        //fn->interfence_graph->node_map.erase("");
        for (auto &it : fn->interfence_graph->node_map) {
            //it.second->num_neighbors = it.second->neighbors.size();
            fn->interfence_graph->sorted_fence_nodes.push_back(it.second);
        }
        std::sort(fn->interfence_graph->sorted_fence_nodes.begin(), fn->interfence_graph->sorted_fence_nodes.end(), fence_cmp);
        return;
    }

    void remove_node(L2::Function* fn) {
        if (fn->interfence_graph->sorted_fence_nodes.empty()) {
            return;
        }
        L2::fence_node* to_remove = fn->interfence_graph->sorted_fence_nodes.back();
        for (auto var : to_remove->neighbors) {
            fn->interfence_graph->node_map[var.get()]->neighbors.erase(*to_remove->var);
            fn->interfence_graph->node_map[var.get()]->num_neighbors -= 1;
        }
        fn->interfence_graph->sorted_fence_nodes.pop_back();
        return;
    }

    void color_function(L2::Function* fn) {
        initiate_color_order();
        bool cant_color = false;
        L2::sort_nodes(fn);
        L2::fence_graph prev_graph = *fn->interfence_graph;
        std::unordered_map<std::string, L2::fence_node*> new_node_map;
        fn->interfence_graph->node_map.clear();
        uint64_t num_nodes = fn->interfence_graph->sorted_fence_nodes.size();
        for (uint64_t i = 0; i < num_nodes; i++) {
            std::vector<uint64_t> neighbor_colors;
            L2::fence_node* cur_fence_node = fn->interfence_graph->sorted_fence_nodes.back();
            if (color_map.find(cur_fence_node->var->get()) != color_map.end()) {
                fn->interfence_graph->node_map[cur_fence_node->var->get()] = cur_fence_node;
                remove_node(fn);
                continue;
            }
            for (auto &var : prev_graph.node_map[cur_fence_node->var->to_string()]->neighbors) {
                if (fn->interfence_graph->node_map.find(var.get()) != fn->interfence_graph->node_map.end()) {
                    if (color_map.find(var.get()) != color_map.end()) {
                        neighbor_colors.push_back(color_map[var.get()]);
                    }
                }
            }
            std::sort(neighbor_colors.begin(), neighbor_colors.end());
            for (uint64_t j = 0; j < num_colors; j++) {
                if (j >= neighbor_colors.size()) {
                    color_map[cur_fence_node->var->get()] = num_colors;
                    cant_color = true;
                    break;
                }
                if (neighbor_colors[j] == j) continue;
                else {
                    color_map[cur_fence_node->var->get()] = j;
                    break;
                }
            }
            fn->interfence_graph->node_map[cur_fence_node->var->get()] = cur_fence_node;
            remove_node(fn);
        }
        if (cant_color) {
            fn->interfence_graph = &prev_graph;
            L2::spill_all(fn);
            generate_liveness_fn(fn);
            generate_fence_fn(fn);
            color_function(fn);
        }
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
                Architecture::RegisterID reg_id = sorted_colors[color_map[var1->to_string()]];
                L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
                i->dst = reg;
            }
        }
        L2::Variable* var2 = dynamic_cast<L2::Variable*>(items[1]);
        if (var2 != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var2);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = sorted_colors[color_map[var2->to_string()]];
                L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
                i->src = reg;
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
                Architecture::RegisterID reg_id = sorted_colors[color_map[var1->to_string()]];
            L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
            i->left = reg;
            }
            
        }
        L2::Variable* var2 = dynamic_cast<L2::Variable*>(items[2]);
        if (var2 != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var2);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = sorted_colors[color_map[var2->to_string()]];
            L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
            i->right = reg;
            }
            
        }
        L2::Variable* var3 = dynamic_cast<L2::Variable*>(items[3]);
        if (var3 != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var3);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = sorted_colors[color_map[var3->to_string()]];
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
                Architecture::RegisterID reg_id = sorted_colors[color_map[var1->to_string()]];
            L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
            i->dst = reg;
            }
            
        }
        L2::Variable* var2 = dynamic_cast<L2::Variable*>(items[1]);
        if (var2 != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var2);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = sorted_colors[color_map[var2->to_string()]];
            L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
            i->left = reg;
            }
            
        }
        L2::Variable* var3 = dynamic_cast<L2::Variable*>(items[3]);
        if (var3 != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var3);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = sorted_colors[color_map[var3->to_string()]];
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
                Architecture::RegisterID reg_id = sorted_colors[color_map[var1->to_string()]];
            L2::Register* reg = new L2::Register(Architecture::to_string(reg_id), reg_id);
            i->left = reg;
            }
            
        }
        L2::Variable* var2 = dynamic_cast<L2::Variable*>(items[2]);
        if (var2 != NULL) {
            L2::Register* reg_tstr = dynamic_cast<L2::Register*>(var2);
            if (reg_tstr == NULL) {
                Architecture::RegisterID reg_id = sorted_colors[color_map[var2->to_string()]];
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
                Architecture::RegisterID reg_id = sorted_colors[color_map[var->to_string()]];
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
        fence_graph_copy = *fn->interfence_graph;
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