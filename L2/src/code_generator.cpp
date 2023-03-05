#include <code_generator.h>
#include <iostream>

namespace L2 {

    std::string output_string;

    L2::code_generator_visitor generator;
    L2::stack_arg_converter stack_arg_er;

    std::string tab_offset;

    uint64_t stack_arg_offset;

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
        output_string += tab_offset + ")\n";
        return;
    }

        void stack_arg_converter::visit(L2::Instruction_return* i) {

        return;
    }
    void stack_arg_converter::visit(L2::Instruction_assignment* i) {
        std::vector<Item*> items = i->get();
        L2::StackArg* src_stack = dynamic_cast<L2::StackArg*>(items[1]);
        if (src_stack != NULL) {
            uint64_t stack_arg_num = src_stack->get()->get();
            L2::Register* rsp_var = new L2::Register("rsp", Architecture::RegisterID::rsp);
            L2::Number* offset = new L2::Number(stack_arg_offset+stack_arg_num);
            L2::MemoryLocation* mem = new L2::MemoryLocation(rsp_var, offset);
            i->src = mem;
        }
        return;
    }
    void stack_arg_converter::visit(L2::Instruction_operation* i) {
    
        return;
    }
    void stack_arg_converter::visit(L2::Instruction_save_cmp* i) {
    
        return;
    }
    void stack_arg_converter::visit(L2::Instruction_cjump* i) {

        return;
    }
    void stack_arg_converter::visit(L2::Instruction_label* i) {

        return;
    }
    void stack_arg_converter::visit(L2::Instruction_goto* i) {

        return;
    }
    void stack_arg_converter::visit(L2::Instruction_call* i) {

        return;
    }
    void stack_arg_converter::visit(L2::Instruction_call_print* i) {

        return;
    }
    void stack_arg_converter::visit(L2::Instruction_call_input* i) {

        return;
    }
    void stack_arg_converter::visit(L2::Instruction_call_allocate* i) {

        return;
    }
    void stack_arg_converter::visit(L2::Instruction_call_tensorError* i) {

        return;
    }

    void stack_arg_converter::visit(L2::Program* p) {
        for (auto fn : p->functions) {
            fn->accept(&stack_arg_er);
        }
        return;
    }

    void stack_arg_converter::visit(L2::Function* fn) {
        stack_arg_offset = fn->locals;
        if (fn->arguments > 6) {
            stack_arg_offset += fn->arguments - 6;
        }
        for (auto instr : fn->instructions) {
            instr->accept(&stack_arg_er);
        }
        return;
    }

    void convert_stack_args(L2::Program &p) {
        p.accept(&stack_arg_er);
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