#pragma once
#include <string>
#include <map>

namespace L3 {

    class Item {
        public:
            std::string s;
    };

    class Fn_Name_Item : Item {

    };

    class Number : Item {

    };

    class Label : Item {

    };

    class Variable : Item {

    };

    class Register : Item {

    };
    
    // ...

    class Visitor;

    class Tile;

    class Tree {
        public:
            Tree();
            Item* root;
            Item* op;
            std::vector<Tree*> leafs;
            Tile* tile;
            //Instruction* instr;
    };

    class Tile {
        public:
            virtual bool matchesTree(Tree*) = 0;
            virtual std::string newInstruction(Instruction* instr) = 0;
            uint64_t num_instructions;
            uint64_t num_nodes;
    };

    // all you gotta do for each tile is file out the matchesTree fn
    // then just instantiate a global vector of all the different kind
    // of tiles whenever u begin tiling, and boom

    class Instruction {
        public:
            virtual void accept(Visitor* v) = 0;
            std::string s;
            Tree* tree;
            bool is_L2;
    };

    class Instruction_return : Instruction {
        public:
            void accept(Visitor* v);
            bool returns_val;
            Item* return_val;
    };

    class Instruction_label : Instruction {
        public:
            void accept(Visitor* v);
    };

    class Instruction_branch : Instruction {
        public:
            void accept(Visitor* v);
            std::vector<Item*> items;
    };

    class Instruction_call : Instruction {
        public:
            void accept(Visitor* v);
            std::vector<Item*> args;
            uint64_t num_args;
            std::string fn_name;
    };

    class Instruction_assignment : Instruction {
        public:
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