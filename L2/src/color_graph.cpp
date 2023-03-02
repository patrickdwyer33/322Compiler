#include <color_graph.h>

namespace L2 {

    std::vector<Architecture::RegisterID> sorted_colors;
    std::vector<Architecture::RegisterID>* cur_color;

    void initiate_color_order(L2::Function* fn) {
        auto caller_save_regs = Architecture::get_caller_save_registers();
        auto callee_save_regs = Architecture::get_callee_save_registers();
        sorted_colors.clear();
        for (auto reg : caller_save_regs) {
            sorted_colors.push_back(reg);
        }
        for (auto reg : callee_save_regs) {
            sorted_colors.push_back(reg);
        }
        return;
    }

    L2::fence_node* remove_node(L2::Function* fn) {
        L2::fence_node* to_remove = fn->sorted_fence_nodes.top();
        if (to_remove == NULL) {
            return to_remove;
        }
        fn->sorted_fence_nodes.pop();
        fn->interfence_graph->node_map.erase(to_remove->var->to_string());
        for (auto var : to_remove->neighbors) {
            var->neighbors.erase(to_remove->var->to_string());
        }
        to_remove->neighbors.clear();
        return to_remove;
    }
    
    bool bigger_node(L2::fence_node left, L2::fence_node right) {
        return left->neighbors.size() > right->neighbors.size();
    }

    void color_function(L2::Function* fn) {
        std::vector<L2::fence_node*> stack;
        L2::fence_graph prev_graph = *fn->interfence_graph;
        L2::fence_node* cur_node = remove_node(fn);
        while (cur_node != NULL) {
            stack.push_back(cur_node);
            cur_node = remove_node(fn);
        }
        cur_color = &sorted_colors;
        for (int i = stack.size() - 1; i >= 0; i++) {
            L2::fence_node cur_fence_node = *stack[i];
            for (auto node : prev_graph.node_map[cur_fence_node.var->to_string()].neighbors) {
                if (fn->interfence_graph->node_map.find(node.var->to_string()) != fn->interfence_graph->node_map.end()) {
                    cur_fence_node.neighbors.insert(*node.var);
                }
            }
            fn->interfence_graph->node_map.insert(std::make_pair(cur_fence_node.var->to_string(), cur_fence_node));
            // now add color to it if you can
            // if you ever can't add color at all, gotta set a bool that we can check later
            // but gotta keep adding em all back in (I think?)
        }
    }

    void color_program(L2::Program &p) {
        for (auto fn : p.functions) {
            color_function(fn);
        }
        return;
    }

}