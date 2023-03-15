#include <tiler.h>
#include <iostream>

namespace L3 {

    Tiler_Visitor tiler_vis;

    std::vector<Tile*> tiles;

    void instatiate_tiles() {
        // hard-codedly instatiate tile vector in order first by num_nodes then by num_instructions (cost)
        tiles.clear();
    }

    void Tiler_Visitor::visit(Instruction_return* i) {
        return;
    }
    void Tiler_Visitor::visit(Instruction_label* i) {
        i->is_L2 = true;
        return;
    }
    void Tiler_Visitor::visit(Instruction_branch* i) {
        for (uint64_t i = 0; i < tiles.size(); i++) {
            tile = tiles[i];
            if (tile.matchesTree(i->tree)) {
                i->s = tile.newInstruction(i);
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
        for (uint64_t i = 0; i < tiles.size(); i++) {
            tile = tiles[i];
            if (tile.matchesTree(i->tree)) {
                i->s = tile.newInstruction(i);
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