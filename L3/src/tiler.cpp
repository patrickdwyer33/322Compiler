#include <tiler.h>
#include <iostream>

namespace L3 {

    Tiler_Visitor tiler_vis;

    std::vector<Tile*> tiles;

    void instatiate_tiles() {
        // hard-codedly instatiate tile vector in order first by num_nodes then by num_instructions (cost)
        tiles.clear();
        Tile_branch_1* t1 = new Tile_branch_1();
        tiles.push_back(t1);
        Tile_branch_2* t2 = new Tile_branch_2();
        tiles.push_back(t1);
        Tile_assignment_2* t3 = new Tile_assignment_2();
        tiles.push_back(t3);
        Tile_assignment_3* t4 = new Tile_assignment_3();
        tiles.push_back(t4);
        Tile_assignment_4_cmp* t5 = new Tile_assignment_4_cmp();
        tiles.push_back(t5);
        Tile_assignment_4_op_1_instr* t6 = new Tile_assignment_4_op_1_instr();
        tiles.push_back(t6);
        Tile_assignment_4_op* t7 = new Tile_assignment_4_op();
        tiles.push_back(t7);
        return;
    }

    void Tiler_Visitor::visit(Instruction_return* i) {
        return;
    }
    void Tiler_Visitor::visit(Instruction_label* i) {
        i->is_L2 = true;
        return;
    }
    void Tiler_Visitor::visit(Instruction_branch* i) {
        for (uint64_t idx = 0; idx < tiles.size(); idx++) {
            auto tile = tiles[idx];
            if (tile->matchesTree(i->tree)) {
                i->s = tile->newInstruction(i);
                i->is_L2 = true;
            }
        }
        if (!i->is_L2) {
            std::cout << "COULDN'T FIND MATCHING TILE!!!!\n";
        }
        return;
    }
    void Tiler_Visitor::visit(Instruction_call* i) {
        return;
    }
    void Tiler_Visitor::visit(Instruction_assignment* i) {
        if (i->tree == NULL) return;
        for (uint64_t idx = 0; idx < tiles.size(); idx++) {
            auto tile = tiles[idx];
            if (tile->matchesTree(i->tree)) {
                i->s = tile->newInstruction(i);
                i->is_L2 = true;
            }
        }
        if (!i->is_L2) {
            std::cout << "COULDN'T FIND MATCHING TILE!!!!\n";
        }
        return;
    }
    void Tiler_Visitor::visit(Function* fn) {
        for (auto instr : fn->instructions) {
            instr->accept(&tiler_vis);
        }
    }
    void Tiler_Visitor::visit(Program* p) {
        instatiate_tiles();
        for (auto fn : p->functions) {
            fn->accept(&tiler_vis);
        }
    }

    void Generate_Tiles(Program &p) {
        p.accept(&tiler_vis);
    }

}