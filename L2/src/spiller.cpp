#include <spiller.h>
#include <vector>
#include <string>
#include <iterator>
#include <iostream>
#include <code_generator.h>

namespace L2 {

    L2::spiller spill_visitor;
    std::string spill_prefix;
    L2::Variable* spill_var;
    uint64_t cur_var_counter;

    void spiller::visit(L2::Instruction_return* i) {

        return;
    }
    void spiller::visit(L2::Instruction_assignment* i) {
        std::vector<L2::Item*> items = i->get();
        L2::Variable* dst = dynamic_cast<L2::Variable*>(items[0]);
        L2::Variable* src = dynamic_cast<L2::Variable*>(items[1]);
        if (dst != NULL) {
            if (*spill_var == *dst) {
                i->contains_var = true;
                i->var_is_modified = true;
                L2::Variable* new_var = new L2::Variable(spill_prefix + std::to_string(cur_var_counter));
                i->dst = new_var;
            }
        } else {
            L2::MemoryLocation* dst_mem = dynamic_cast<L2::MemoryLocation*>(items[0]);
            if (dst_mem != NULL) {
                dst = static_cast<L2::Variable*>(dst_mem->get()[0]);
                if (*spill_var == *dst) {
                    i->contains_var = true;
                    i->var_is_read = true;
                    L2::Variable* new_var = new L2::Variable(spill_prefix + std::to_string(cur_var_counter));
                    dst_mem->base_var = new_var;
                    i->dst = dst_mem;
                }
            }
        }
        if (src != NULL) {
            if (*spill_var == *src) {
                i->contains_var = true;
                i->var_is_read = true;
                L2::Variable* new_var = new L2::Variable(spill_prefix + std::to_string(cur_var_counter));
                i->src = new_var;
            }
        } else {
            L2::MemoryLocation* src_mem = dynamic_cast<L2::MemoryLocation*>(items[1]);
            if (src_mem != NULL) {
                src = static_cast<L2::Variable*>(src_mem->get()[0]);
                if (*spill_var == *src) {
                    i->contains_var = true;
                    i->var_is_read = true;
                    L2::Variable* new_var = new L2::Variable(spill_prefix + std::to_string(cur_var_counter));
                    src_mem->base_var = new_var;
                    i->src = src_mem;
                }
            }
        }
        return;
    }
    void spiller::visit(L2::Instruction_operation* i) {
        std::vector<L2::Item*> items = i->get();
        L2::Variable* left_var = dynamic_cast<L2::Variable*>(items[0]);
        L2::Variable* right_var = dynamic_cast<L2::Variable*>(items[2]);
        L2::Variable* lea_reg = dynamic_cast<L2::Variable*>(items[3]);
        L2::NullItem* lea_null_tstr = dynamic_cast<L2::NullItem*>(items[3]);
        if (left_var != NULL) {
            if (*spill_var == *left_var) {
                i->contains_var = true;
                i->var_is_modified = true;
                if (lea_null_tstr != NULL) {
                    i->var_is_read = true;
                }
                L2::Variable* new_var = new L2::Variable(spill_prefix + std::to_string(cur_var_counter));
                i->left = new_var;
            }
        } else {
            L2::MemoryLocation* left_mem = dynamic_cast<L2::MemoryLocation*>(items[0]);
            if (left_mem != NULL) {
                left_var = static_cast<L2::Variable*>(left_mem->get()[0]);
                if (*spill_var == *left_var) {
                    i->contains_var = true;
                    i->var_is_read = true;
                    L2::Variable* new_var = new L2::Variable(spill_prefix + std::to_string(cur_var_counter));
                    left_mem->base_var = new_var;
                    i->left = left_mem;
                }
            }
        }
        if (right_var != NULL) {
            if (*spill_var == *right_var) {
                i->contains_var = true;
                i->var_is_read = true;
                L2::Variable* new_var = new L2::Variable(spill_prefix + std::to_string(cur_var_counter));
                i->right = new_var;
            }
        } else {
            L2::MemoryLocation* right_mem = dynamic_cast<L2::MemoryLocation*>(items[0]);
            if (right_mem != NULL) {
                right_var = static_cast<L2::Variable*>(right_mem->get()[0]);
                if (*spill_var == *right_var) {
                    i->contains_var = true;
                    i->var_is_read = true;
                    L2::Variable* new_var = new L2::Variable(spill_prefix + std::to_string(cur_var_counter));
                    right_mem->base_var = new_var;
                    i->right = right_mem;
                }
            }
        }
        if (lea_reg != NULL) {
            if (*spill_var == *lea_reg) {
                i->contains_var = true;
                i->var_is_read = true;
                L2::Variable* new_var = new L2::Variable(spill_prefix + std::to_string(cur_var_counter));
                i->lea_reg = new_var;
            }
        }
        return;
    }
    void spiller::visit(L2::Instruction_save_cmp* i) {
        std::vector<L2::Item*> items = i->get();
        L2::Variable* dst_var = dynamic_cast<L2::Variable*>(items[0]);
        L2::Variable* left_var = dynamic_cast<L2::Variable*>(items[1]);
        L2::Variable* right_var = dynamic_cast<L2::Variable*>(items[3]);
        if (dst_var != NULL) {
            if (*spill_var == *left_var) {
                i->contains_var = true;
                i->var_is_modified = true;
                L2::Variable* new_var = new L2::Variable(spill_prefix + std::to_string(cur_var_counter));
                i->dst = new_var;
            }
        }
        if (left_var != NULL) {
            if (*spill_var == *left_var) {
                i->contains_var = true;
                i->var_is_read = true;
                L2::Variable* new_var = new L2::Variable(spill_prefix + std::to_string(cur_var_counter));
                i->left = new_var;
            }
        }
        if (right_var != NULL) {
            if (*spill_var == *right_var) {
                i->contains_var = true;
                i->var_is_read = true;
                L2::Variable* new_var = new L2::Variable(spill_prefix + std::to_string(cur_var_counter));
                i->right = new_var;
            }
        }
        return;
    }
    void spiller::visit(L2::Instruction_cjump* i) {
        std::vector<L2::Item*> items = i->get();
        L2::Variable* left_var = dynamic_cast<L2::Variable*>(items[0]);
        L2::Variable* right_var = dynamic_cast<L2::Variable*>(items[2]);
        if (left_var != NULL) {
            if (*spill_var == *left_var) {
                i->contains_var = true;
                i->var_is_read = true;
                L2::Variable* new_var = new L2::Variable(spill_prefix + std::to_string(cur_var_counter));
                i->left = new_var;
            }
        }
        if (right_var != NULL) {
            if (*spill_var == *right_var) {
                i->contains_var = true;
                i->var_is_read = true;
                L2::Variable* new_var = new L2::Variable(spill_prefix + std::to_string(cur_var_counter));
                i->right = new_var;
            }
        }
        return;
    }
    void spiller::visit(L2::Instruction_label* i) {

        return;
    }
    void spiller::visit(L2::Instruction_goto* i) {
        
        return;
    }
    void spiller::visit(L2::Instruction_call* i) {
        std::vector<Item*> items = i->get();
        L2::Variable* var = dynamic_cast<L2::Variable*>(items[0]);
        if (var != NULL) {
            if (*spill_var == *var) {
                i->contains_var = true;
                i->var_is_read = true;
                L2::Variable* new_var = new L2::Variable(spill_prefix + std::to_string(cur_var_counter));
                i->fn = new_var;
            }
        }
        L2::Variable* var2 = dynamic_cast<L2::Variable*>(items[1]);
        if (var2 != NULL) {
            if (*spill_var == *var2) {
                i->contains_var = true;
                i->var_is_read = true;
                L2::Variable* new_var = new L2::Variable(spill_prefix + std::to_string(cur_var_counter));
                i->arg = new_var;
            }
        }
        return;
    }
    void spiller::visit(L2::Instruction_call_print* i) {

        return;
    }
    void spiller::visit(L2::Instruction_call_input* i) {

        return;
    }
    void spiller::visit(L2::Instruction_call_allocate* i) {

        return;
    }
    void spiller::visit(L2::Instruction_call_tensorError* i) {

        return;
    }

    void spiller::visit(L2::Program* p) {
        /*for (auto fn : p->functions) {
            fn->accept(&spill_visitor);
        }*/
        return;
    }

    void spiller::visit(L2::Function* fn) {

        std::vector<L2::Instruction*> cur_instructions = fn->instructions;
        uint64_t num_added = 0;
        cur_var_counter = 0;
        //auto last_added_it = fn->instructions.begin();

        for (int idx = 0; idx < cur_instructions.size(); idx++) {
            L2::Instruction* instr = cur_instructions[idx];
            instr->contains_var = false;
            instr->var_is_modified = false;
            instr->var_is_read = false;
            instr->accept(&spill_visitor);
            if (instr->contains_var) {
                // maybe can't use same object for two things?
                std::string cur_var_name = spill_prefix + std::to_string(cur_var_counter);
                L2::Register* rsp_item = new L2::Register("rsp", Architecture::RegisterID::rsp);
                L2::Number* offset = new L2::Number(fn->locals*8);
                L2::MemoryLocation* stack_spot = new L2::MemoryLocation(rsp_item, offset);
                L2::Variable* var = new L2::Variable(cur_var_name);
                L2::Instruction_assignment* new_ass_to_load = new L2::Instruction_assignment(var, stack_spot);
                L2::Instruction_assignment* new_ass_to_store = new L2::Instruction_assignment(stack_spot, var);
                auto it = fn->instructions.begin();
                if (instr->var_is_read) {
                    fn->instructions.insert(std::next(it, idx + num_added), new_ass_to_load);
                    num_added++;
                    fn->was_created[*var] = true;
                }
                if (instr->var_is_modified) {
                    it = fn->instructions.begin();
                    fn->instructions.insert(std::next(it, idx + num_added + 1), new_ass_to_store);
                    num_added++;
                    fn->was_created[*var] = true;
                }
                cur_var_counter++;
                //it = fn->instructions.begin();
                //last_added_it = std::next(it, idx + num_added);
            }
        }
        //fn->instructions.erase(last_added_it);
        if (num_added > 0) {
            fn->locals++;
        }
        return;
    }

    void spill(L2::Program &p) {
        spill_prefix = p.prefix_var->to_string();
        spill_var = p.to_spill_var;
        for (auto& fn : p.functions) {
            fn->accept(&spill_visitor);
        }
        return;
    }

    void spill_all(L2::Function* fn) {
        char cur_prefix_char = 'A';
        std::string cur_prefix;
        for (auto it : fn->interfence_graph->node_map) {
            cur_prefix.push_back(cur_prefix_char);
            spill_var = &it.second.var;
            spill_prefix = cur_prefix;
            fn->accept(&spill_visitor);
            cur_prefix.pop_back();
            cur_prefix_char++;
            if (cur_prefix_char > 'Z') {
                cur_prefix_char = 'A';
                cur_prefix.push_back(cur_prefix_char);
            }
        }
        return;
    }

    void print_spill(L2::Program &p) {
        for (auto& fn : p.functions) {
            L2::print_function(*fn);
        }
        return;
    }

}