#pragma once
#include <string>
#include <vector>
#include <map>

namespace L3 {

    class Instruction;

    class Item {
        public:
            virtual ~A() = default;
            std::string s;
    };

    class Fn_Name_Item : public Item {
        public:
            Fn_Name_Item(std::string s);
    };

    class Number : public Item {
        public:
            Number(uint64_t n);
            uint64_t val;
    };

    class Label : public Item {
        public:
            Label(std::string s);
    };

    class CmpOP : public Item {
        public:
            //void nothing();
            CmpOP(std::string s);
    };

    class OP : public Item {
        public:
            //void nothing();
            OP(std::string s);
    };

    class Barrier : public Item {
        public:
            Barrier();
    };

    class Return_val : public Item {
        public:
            Return_val(L3::Item* val);
            L3::Item* val;
    };

    class Branch_val : public Item {
        public:
            Branch_val(Item* val);
            Item* val;
    };

    class Call_item : public Item {
        public:
            Call_item(uint64_t num_args, std::vector<Item*> args);
            uint64_t num_args;
            std::vector<Item*> args;
    };

    class Store : public Item {
        public:
            Store();
    };

    class Load : public Item {
        public:
            Load();
    };

    class Arrow : public Item {
        public:
            Arrow();
    };

    class Variable : public Item {
        public:
            Variable(std::string s);
    };

    
    // ...

    class Visitor;

    class Tile;

    class Tree {
        public:
            Tree();
            Item* root;
            Item* op;
            std::vector<Item*> leafs;
            Tile* tile;
            //Instruction* instr;
    };

    class Tile {
        public:
            virtual bool matchesTree(Tree* tree) = 0;
            virtual std::string newInstruction(Instruction* instr) = 0;
            uint64_t num_instructions;
            uint64_t num_nodes;
    };

    class Tile_branch_1 : public Tile {
        public:
            Tile_branch_1();
            bool matchesTree(Tree* tree);
            std::string newInstruction(Instruction* instr);
    };

    class Tile_branch_2 : public Tile {
        public:
            Tile_branch_2();
            bool matchesTree(Tree* tree);
            std::string newInstruction(Instruction* instr);
    };

    class Tile_assignment_2 : public Tile {
        public:
            Tile_assignment_2();
            bool matchesTree(Tree* tree);
            std::string newInstruction(Instruction* instr);
    };

    class Tile_assignment_3 : public Tile {
        public:
            Tile_assignment_3();
            bool matchesTree(Tree* tree);
            std::string newInstruction(Instruction* instr);
    };

    class Tile_assignment_4_cmp : public Tile {
        public:
            Tile_assignment_4_cmp();
            bool matchesTree(Tree* tree);
            std::string newInstruction(Instruction* instr);
    };

    class Tile_assignment_4_op : public Tile {
        public:
            Tile_assignment_4_op();
            bool matchesTree(Tree* tree);
            std::string newInstruction(Instruction* instr);
    };

    class Tile_assignment_4_op_1_instr : public Tile {
        public:
            Tile_assignment_4_op_1_instr();
            bool matchesTree(Tree* tree);
            std::string newInstruction(Instruction* instr);
    };

    // all you gotta do for each tile is fill out the matchesTree fn
    // then just instantiate a global vector of all the different kind
    // of tiles whenever u begin tiling, and boom

    class Instruction {
        public:
            virtual void accept(Visitor* v) = 0;
            std::string s;
            Tree* tree;
            bool is_L2;
    };

    class Instruction_return : public Instruction {
        public:
            Instruction_return();
            Instruction_return(Item* ret_val);
            void accept(Visitor* v);
            bool returns_val;
            Item* return_val;
    };

    class Instruction_label : public Instruction {
        public:
            Instruction_label(Item* label);
            void accept(Visitor* v);
            Item* label;
    };

    class Instruction_branch : public Instruction {
        public:
            Instruction_branch(Item* label);
            Instruction_branch(Item* label, Item* val);
            void accept(Visitor* v);
            Item* label;
            Item* val;
            bool has_val;
    };

    class Instruction_call : public Instruction {
        public:
            Instruction_call(uint64_t num_args, std::vector<Item*> args);
            void accept(Visitor* v);
            std::vector<Item*> args;
            uint64_t num_args;
            std::string fn_name;
    };

    class Instruction_assignment : public Instruction {
        public:
            Instruction_assignment(Item* left, std::vector<Item*> right);
            void accept(Visitor* v);
            bool is_store;
            bool is_load;
            bool is_call_store;
            Instruction_call* call_instr;
            Item* left;
            std::vector<Item*> right;
    };

    class Function {
        public:
            void accept(Visitor* v);
            std::string name;
            std::vector<Item*> vars;
            //std::vector<std::vector<Instruction*>> raw_contexts;
            std::vector<Instruction*> instructions;
            std::vector<Tile*> tiles;
            uint64_t num_args;
    };

    class Program {
        public:
            void accept(Visitor* v);
            std::vector<Function*> functions; 
    };

    class Visitor {
        public:
            virtual void visit(Instruction_return* i) = 0;
            virtual void visit(Instruction_label* i) = 0;
            virtual void visit(Instruction_branch* i) = 0;
            virtual void visit(Instruction_call* i) = 0;
            virtual void visit(Instruction_assignment* i) = 0;
            virtual void visit(Function* fn) = 0;
            virtual void visit(Program* p) = 0;
    };
}