#include <L3.h>

#include <string>
#include <vector>
#include <cassert>

namespace L3 {

    Fn_Name_Item::Fn_Name_Item(std::string s) {
        this->s = s;
    }

    Number::Number(uint64_t n) {
        this->val = n;
        this->s = std::to_string(n);
    }

    Label::Label(std::string s) {
        this->s = s;
    }

    CmpOP::CmpOP(std::string s) {
        this->s = s;
    }

    /*void CmpOP::nothing() {
        return;
    }*/

    OP::OP(std::string s) {
        this->s = s;
    }

    /*void OP::nothing() {
        return;
    }*/

    Barrier::Barrier() {}

    Return_val::Return_val(L3::Item* val) {
        this->val = val;
    }

    Branch_val::Branch_val(Item* val) {
        this->val = val;
    }

    Call_item::Call_item(uint64_t num_args, std::vector<Item*> args, std::string fn_name) {
        this->num_args = num_args;
        this->args = args;
        this->fn_name = fn_name;
    }

    Store::Store() {}

    Load::Load() {}

    Arrow::Arrow() {}

    Variable::Variable(std::string s) {
        this->s = s;
    }

    Tree::Tree() {
        this->root = NULL;
        this->op = NULL;
        this->leafs = {};
        this->tile = NULL;
    }

    Tile_branch_1::Tile_branch_1() {
        this->num_instructions = 1;
        this->num_nodes = 1;
    }

    bool Tile_branch_1::matchesTree(Tree* tree) {
        if (tree->root != NULL) return false;
        if (tree->leafs.size() == 1) {
            return true;
        }
        return false;
    }

    std::string Tile_branch_1::newInstruction(Instruction* instr) {
        Instruction_branch* branch_instr = dynamic_cast<Instruction_branch*>(instr);
        assert(branch_instr != NULL);
        return "goto " + branch_instr->label->s;
    }

    Tile_branch_2::Tile_branch_2() {
        this->num_instructions = 1;
        this->num_nodes = 2;
    }

    bool Tile_branch_2::matchesTree(Tree* tree) {
        if (tree->root != NULL) return false;
        if (tree->leafs.size() == 2) {
            return true;
        }
        return false;
    }

    std::string Tile_branch_2::newInstruction(Instruction* instr) {
        Instruction_branch* branch_instr = dynamic_cast<Instruction_branch*>(instr);
        assert(branch_instr != NULL);
        return "cjump " + branch_instr->val->s + " = 1 " + branch_instr->label->s;
    }

    Tile_assignment_2::Tile_assignment_2() {
        this->num_instructions = 1;
        this->num_nodes = 2;
    }

    bool Tile_assignment_2::matchesTree(Tree* tree) {
        if (tree->root == NULL) return false;
        if (tree->leafs.size() == 2) {
            return true;
        }
        return false;
    }

    std::string Tile_assignment_2::newInstruction(Instruction* instr) {
        return instr->s;
    }

    Tile_assignment_3::Tile_assignment_3() {
        this->num_instructions = 1;
        this->num_nodes = 3;
    }

    bool Tile_assignment_3::matchesTree(Tree* tree) {
        if (tree->root == NULL) return false;
        if (tree->leafs.size() == 3) {
            return true;
        }
        return false;
    }

    std::string Tile_assignment_3::newInstruction(Instruction* instr) {
        Instruction_assignment* instr_ass = dynamic_cast<Instruction_assignment*>(instr);
        assert(instr_ass != NULL);
        if (instr_ass->is_call_store) {
            return "CALL ERRORRR in tile assignment 3";
        }
        else if (instr_ass->is_store) {
            return "mem " + instr_ass->left->s + " 0 <- " + instr_ass->right[0]->s;
        }
        else if (instr_ass->is_load) {
            return instr_ass->left->s + " <- " + "mem " + instr_ass->right[0]->s + " 0";
        }
        return "ERRRORR in tile assignment 3";
    }

    Tile_assignment_4_cmp::Tile_assignment_4_cmp() {
        this->num_instructions = 1;
        this->num_nodes = 4;
    }

    bool Tile_assignment_4_cmp::matchesTree(Tree* tree) {
        if (tree->root == NULL) return false;
        if (tree->leafs.size() == 4) {
            CmpOP* cmp = dynamic_cast<CmpOP*>(tree->leafs[1]);
            if (cmp != NULL) {
                return true;
            }
            return false;
        }
        return false;
    }

    std::string Tile_assignment_4_cmp::newInstruction(Instruction* instr) {
        Instruction_assignment* instr_ass = dynamic_cast<Instruction_assignment*>(instr);
        assert(instr_ass != NULL);
        CmpOP* cmp = dynamic_cast<CmpOP*>(instr_ass->right[1]);
        assert(cmp != NULL);
        std::string output;
        output = instr_ass->left->s + " <- ";
        if (cmp->s == ">" || cmp->s == ">=") {
            Item* temp = instr_ass->right[0];
            instr_ass->right[0] = instr_ass->right[2];
            instr_ass->right[2] = temp;
            if (cmp->s == ">") {
                cmp->s = "<";
            }
            else {
                cmp->s = "<=";
            }
            instr_ass->right[1] = cmp;
        }
        output += instr_ass->right[0]->s + " " + instr_ass->right[1]->s + " " + instr_ass->right[2]->s;
        return output;
    }

    Tile_assignment_4_op::Tile_assignment_4_op() {
        this->num_instructions = 2;
        this->num_nodes = 4;
    }

    bool Tile_assignment_4_op::matchesTree(Tree* tree) {
        if (tree->root == NULL) return false;
        if (tree->leafs.size() == 4) {
            OP* op = dynamic_cast<OP*>(tree->leafs[1]);
            if (op != NULL) {
                return true;
            }
            return false;
        }
        return false;
    }

    std::string Tile_assignment_4_op::newInstruction(Instruction* instr) {
        Instruction_assignment* instr_ass = dynamic_cast<Instruction_assignment*>(instr);
        assert(instr_ass != NULL);
        OP* op = dynamic_cast<OP*>(instr_ass->right[1]);
        assert(op != NULL);
        std::string output;
        output = instr_ass->left->s + " <- " + instr_ass->right[0]->s + "\n" + instr_ass->left->s + " " + instr_ass->right[1]->s + "= " + instr_ass->right[2]->s;
        return output;
    }

    Tile_assignment_4_op_1_instr::Tile_assignment_4_op_1_instr() {
        this->num_instructions = 1;
        this->num_nodes = 4;
    }

    bool Tile_assignment_4_op_1_instr::matchesTree(Tree* tree) {
        if (tree->root == NULL) return false;
        if (tree->leafs.size() == 4) {
            OP* op = dynamic_cast<OP*>(tree->leafs[1]);
            if (op != NULL) {
                Number* n1 = dynamic_cast<Number*>(tree->leafs[0]);
                Number* n2 = dynamic_cast<Number*>(tree->leafs[2]);
                if ((tree->root->s == tree->leafs[0]->s && n2 != NULL) || (n1 != NULL && tree->root->s == tree->leafs[0]->s)) {
                    return true;
                }
                return false;
            }
            return false;
        }
        return false;
    }

    std::string Tile_assignment_4_op_1_instr::newInstruction(Instruction* instr) {
        Instruction_assignment* instr_ass = dynamic_cast<Instruction_assignment*>(instr);
        assert(instr_ass != NULL);
        OP* op = static_cast<OP*>(instr_ass->right[1]);
        Number* n1 = dynamic_cast<Number*>(instr_ass->right[0]);
        Number* n2 = dynamic_cast<Number*>(instr_ass->right[2]);
        if (n1 != NULL) {
            return instr_ass->left->s + " " + instr_ass->right[1]->s + "= " + instr_ass->right[0]->s;
        }
        if (n2 != NULL) {
            return instr_ass->left->s + " " + instr_ass->right[1]->s + "= " + instr_ass->right[2]->s;
        }
        return "ERERERERE in Tile_assignment_4_op_1_instr";
    }

    Instruction_return::Instruction_return() {
        this->returns_val = false;
        this->is_L2 = false;
    }

    Instruction_return::Instruction_return(Item* ret_val) {
        this->returns_val = true;
        this->return_val = ret_val;
        this->is_L2 = false;
    }

    void Instruction_return::accept(Visitor* v) {
        v->visit(this);
    }

    Instruction_label::Instruction_label(Item* label) : label(label) {}
    void Instruction_label::accept(Visitor* v) {
        v->visit(this);
    }

    Instruction_branch::Instruction_branch(Item* label) : has_val(false) {this->is_L2 = false;}
    Instruction_branch::Instruction_branch(Item* label, Item* val) : has_val(true) {
        this->label = label;
        this->is_L2 = false;
    }
    void Instruction_branch::accept(Visitor* v) {
        v->visit(this);
    }


    Instruction_call::Instruction_call(uint64_t num_args, std::vector<Item*> args, std::string fn_name) : num_args(num_args), args(args) 
    {this->is_L2 = false;
    this->fn_name = fn_name;}
    void Instruction_call::accept(Visitor* v) {
        v->visit(this);
    }


    Instruction_assignment::Instruction_assignment(Item* left, std::vector<Item*> right) :
        is_store(false), is_load(false), is_call_store(false), left(left), right(right) {
        call_instr = NULL;
        this->is_L2 = false;
    }
    void Instruction_assignment::accept(Visitor* v) {
        v->visit(this);
    }

    void Function::accept(Visitor* v) {
        v->visit(this);
    }

    void Program::accept(Visitor* v) {
        v->visit(this);
    }

}
