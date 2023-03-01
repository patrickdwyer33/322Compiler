#include <code_generator.h>
#include <iostream>

namespace L2 {

    std::string output_string;

    L2::code_generator_visitor generator;

    std::string tab_offset;

    void code_generator_visitor::visit(L2::Instruction_return* i) {
        output_string += "return";
        return;
    }
    void code_generator_visitor::visit(L2::Instruction_assignment* i) {
        std::vector<Item*> items = i->get();
        output_string += items[0]->to_string() + " <- " + items[1]->to_string();
        return;
    }
    void code_generator_visitor::visit(L2::Instruction_operation* i) {
        std::vector<Item*> items = i->get();
        L2::NullItem* right_null_tstr = dynamic_cast<L2::NullItem*>(items[2]);
        for (int i = 0; i < items.size(); i++) {
            L2::Item* item = items[i];
            L2::NullItem* null_tstr = dynamic_cast<L2::NullItem*>(item);
            if (null_tstr == NULL && i > 0 && right_null_tstr == NULL) {
                output_string += " ";
            }
            output_string += item->to_string();
        }
        return;
    }
    void code_generator_visitor::visit(L2::Instruction_save_cmp* i) {
        std::vector<Item*> items = i->get();
        output_string += items[0]->to_string() + " <-";
        for (int i = 1; i < items.size(); i++) {
            output_string += " " + items[i]->to_string();
        }
        return;
    }
    void code_generator_visitor::visit(L2::Instruction_cjump* i) {
        std::vector<Item*> items = i->get();
        output_string += "cjump";
        for (auto item : items) {
            output_string += " " + item->to_string();
        }
        return;
    }
    void code_generator_visitor::visit(L2::Instruction_label* i) {
        output_string += i->get()->to_string();
        return;
    }
    void code_generator_visitor::visit(L2::Instruction_goto* i) {
        output_string += "goto " + i->get()->to_string();
        return;
    }
    void code_generator_visitor::visit(L2::Instruction_call* i) {
        std::vector<Item*> items = i->get();
        output_string += "call " + items[0]->to_string() + " " + items[1]->to_string();
        return;
    }
    void code_generator_visitor::visit(L2::Instruction_call_print* i) {
        output_string += "call print 1";
        return;
    }
    void code_generator_visitor::visit(L2::Instruction_call_input* i) {
        output_string += "call input 0";
        return;
    }
    void code_generator_visitor::visit(L2::Instruction_call_allocate* i) {
        output_string += "call allocate 2";
        return;
    }
    void code_generator_visitor::visit(L2::Instruction_call_tensorError* i) {
        output_string += "call tensor-error " + i->get()->to_string();
        return;
    }

    void code_generator_visitor::visit(L2::Program* p) {
        output_string = "";
        tab_offset = "\t";
        output_string += "(" + p->entryPointLabel + "\n";
        for (auto fn : p->functions) {
            fn->accept(&generator);
        }
        output_string += ")\n";
        return;
    }

    void code_generator_visitor::visit(L2::Function* fn) {
        output_string += tab_offset + "(" + fn->name + "\n" + tab_offset + "\t" + std::to_string(fn->arguments) + " " + std::to_string(fn->locals) + "\n";
        for (auto& instr : fn->instructions) {
            output_string += tab_offset + "\t";
            instr->accept(&generator);
            output_string += "\n";
        }
        output_string += ")\n";
        return;
    }

    std::string generate_code(L2::Program &p) {
        p.accept(&generator);
        return output_string;
    }
    void print_function(L2::Function &fn) {
        output_string = "";
        tab_offset = "";
        fn.accept(&generator);
        std::cout << output_string;
    }
}