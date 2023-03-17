#include <tree_gen.h>

namespace L3 {

    Tree_Visitor tree_vis;

    void Tree_Visitor::visit(Instruction_return* i) {
        return;
    }
    void Tree_Visitor::visit(Instruction_label* i) {
        return;
    }
    void Tree_Visitor::visit(Instruction_branch* i) {
        Tree* tree = new Tree();
        tree->leafs.push_back(i->label);
        if (i->has_val) {
            tree->leafs.push_back(i->val);
        }
    }
    void Tree_Visitor::visit(Instruction_call* i) {
        return;
    }
    void Tree_Visitor::visit(Instruction_assignment* i) {
        if (i->is_call_store) return;
        Tree* tree = new Tree();
        tree->root = i->left;
        tree->leafs.clear();
        if (i->right.size() > 1) {
            tree->op = i->right[1];
            tree->leafs.push_back(i->right[0]);
            tree->leafs.push_back(i->right[2]);
            if (i->is_store) {
                Store* store = new Store();
                tree->leafs.push_back(store);
            }
            if (i->is_load) {
                Load* load = new Load();
                tree->leafs.push_back(load);
            }
        }
        else {
            tree->leafs.push_back(i->right[0]);
        }
        return;
    }
    void Tree_Visitor::visit(Function* fn) {
        for (auto instr : fn->instructions) {
            instr->accept(&tree_vis);
        }
        return;
    }
    void Tree_Visitor::visit(Program* p) {
        for (auto fn : p->functions) {
            fn->accept(&tree_vis);
        }
        return;
    }

    void Generate_Trees(Program &p) {
        p.accept(&tree_vis);
    }
}