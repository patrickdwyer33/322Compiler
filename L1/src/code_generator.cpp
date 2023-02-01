#include <string>
#include <iostream>
#include <fstream>
#include <L1.h>
#include <typeinfo>

#include <code_generator.h>

using namespace std;

namespace Generator {

  std::ofstream outputFile;

  Assembly_visitor* generator;

  std::string NullItemName = "asdf";
  std::string NumberName = "asdf";
  std::string LabelName = "asdf";
  std::string RegisterName = "asdf";
  std::string MemoryLocationName = "asdf";
  std::string OperationName = "asdf";
  std::string CmpOperationName = "asdf";

  void generate_code(L1::Program p) {

    /* 
     * Open the output file.
     */ 
    outputFile.open("prog.S");
   
    /* 
     * Generate target code
     */
    p.accept(generator);

    /* 
     * Close the output file.
     */ 
    outputFile.close();
   
    return;
  }

  void save_calle_saved_registers() {
    outputFile << "\t\tpushq %rbx" << std::endl;
    outputFile << "\t\tpushq %rbp" << std::endl;
    outputFile << "\t\tpushq %r12" << std::endl;
    outputFile << "\t\tpushq %r13" << std::endl;
    outputFile << "\t\tpushq %r14" << std::endl;
    outputFile << "\t\tpushq %r15" << std::endl;
    return;
  }

  void restore_calle_saved_registers() {
    outputFile << "\t\tpopq %r15" << std::endl;
    outputFile << "\t\tpopq %r14" << std::endl;
    outputFile << "\t\tpopq %r13" << std::endl;
    outputFile << "\t\tpopq %r12" << std::endl;
    outputFile << "\t\tpopq %rbp" << std::endl;
    outputFile << "\t\tpopq %rbx" << std::endl;
    return;
  }

  // w <- s
  void generate_assignment(L1::Register* r, L1::Number* n) {
    outputFile << "\t\tmovq $" << std::to_string(n->get()) << ", %" << Architecture::to_string(r->get()) << std::endl;
    return;
  }
  void generate_assignment(L1::Register* r, L1::Label* label) {
    outputFile << "\t\tmovq $" << label->get() << ", %" << Architecture::to_string(r->get()) << std::endl;
    return;
  }
  void generate_assignment(L1::Register* r1, L1::Register* r2) {
    outputFile << "\t\tmovq %" << Architecture::to_string(r2->get()) << ", %" << Architecture::to_string(r1->get()) << std::endl;
    return;
  }
  // w <- mem x M
  void generate_assignment(L1::Register* r, L1::MemoryLocation* mem) {
    auto mem_info = mem->get();
    outputFile << "\t\tmovq " << std::to_string(std::get<1>(mem_info)->get())<< "(%" << Architecture::to_string(std::get<0>(mem_info)->get()) << ") %" << Architecture::to_string(r->get()) << std::endl;
    return;
  }
  // mem x M <- s
  void generate_assignment(L1::MemoryLocation* mem, L1::Number* n) {
    auto mem_info = mem->get();
    outputFile << "\t\tmovq $" << std::to_string(n->get()) << ", " << std::to_string(std::get<1>(mem_info)->get())<< "(%" << Architecture::to_string(std::get<0>(mem_info)->get()) << ")" << std::endl;
    return;
  }
  void generate_assignment(L1::MemoryLocation* mem, L1::Label* label) {
    auto mem_info = mem->get();
    outputFile << "\t\tmovq $_" << label->get().substr(1) << ", " << std::to_string(std::get<1>(mem_info)->get())<< "(%" << Architecture::to_string(std::get<0>(mem_info)->get()) << ")" << std::endl;
    return;
  }
  void generate_assignment(L1::MemoryLocation* mem, L1::Register* r) {
    auto mem_info = mem->get();
    outputFile << "\t\tmovq %" << Architecture::to_string(r->get()) << ", " << std::to_string(std::get<1>(mem_info)->get())<< "(%" << Architecture::to_string(std::get<0>(mem_info)->get()) << ")" << std::endl;
    return;
  }
  // w aop t, w sop sx, w sop N
  void generate_op(L1::Register* r1, L1::Operation* op, L1::Register* r2, L1::NullItem* blank, L1::NullItem* blank2) {
    auto op_info = Architecture::get_op_instr(op->get());
    std::string instruction = std::get<0>(op_info);
    bool is_shift = std::get<1>(op_info);
    outputFile << "\t\t" << instruction << " %";
    if (is_shift) {
      outputFile << Architecture::get_eight_bit(r2->get());
    } else {
      outputFile << Architecture::to_string(r2->get());
    }
    outputFile << ", %" << Architecture::to_string(r1->get()) << std::endl;
    return;
  }
  void generate_op(L1::Register* r, L1::Operation* op, L1::Number* n, L1::NullItem* blank, L1::NullItem* blank2) {
    auto op_info = Architecture::get_op_instr(op->get());
    std::string instruction = std::get<0>(op_info);
    outputFile << "\t\t" << instruction << " $" << std::to_string(n->get());
    outputFile << ", %" << Architecture::to_string(r->get()) << std::endl;
    return;
  }
  // mem x M += t, mem x M -= t
  void generate_op(L1::MemoryLocation* mem, L1::Operation* op, L1::Register* r, L1::NullItem* blank, L1::NullItem* blank2) {
    auto op_info = Architecture::get_op_instr(op->get());
    auto mem_info = mem->get();
    std::string instruction = std::get<0>(op_info);
    outputFile << "\t\t" << instruction << " %" << Architecture::to_string(r->get()) << ", %" << std::to_string(std::get<1>(mem_info)->get()) << "(" << Architecture::to_string(std::get<0>(mem_info)->get()) << ")" << std::endl;
    return;
  }
  void generate_op(L1::MemoryLocation* mem, L1::Operation* op, L1::Number* n, L1::NullItem* blank, L1::NullItem* blank2) {
    auto op_info = Architecture::get_op_instr(op->get());
    auto mem_info = mem->get();
    std::string instruction = std::get<0>(op_info);
    outputFile << "\t\t" << instruction << " $" << std::to_string(n->get()) << ", %" << std::to_string(std::get<1>(mem_info)->get()) << "(" << Architecture::to_string(std::get<0>(mem_info)->get()) << ")" << std::endl;
    return;
  }
  // w += mem x M, w -= mem x M
  void generate_op(L1::Register* r, L1::Operation* op, L1::MemoryLocation* mem, L1::NullItem* blank, L1::NullItem* blank2) {
    auto op_info = Architecture::get_op_instr(op->get());
    auto mem_info = mem->get();
    std::string instruction = std::get<0>(op_info);
    outputFile << "\t\t" << instruction << " %" << std::to_string(std::get<1>(mem_info)->get()) << "(" << Architecture::to_string(std::get<0>(mem_info)->get()) << ")" << ", %" << Architecture::to_string(r->get()) << std::endl;
    return;
  }
  // w <- t cmp t
  void generate_save_cmp(L1::Register* dst, L1::Register* r1, L1::CmpOperation* cmpOP, L1::Register* r2) {
    outputFile << "\t\t" << "cmpq %" << Architecture::to_string(r2->get()) << ", %" << Architecture::to_string(r1->get()) << std::endl;
    outputFile << "\t\tset" << Architecture::get_cmp_instr(cmpOP->get()) << " %" << Architecture::get_eight_bit(dst->get()) << std::endl;
    outputFile << "\t\tmovzbq %" << Architecture::get_eight_bit(dst->get()) << ", %" << Architecture::to_string(dst->get()) << std::endl;
    return;
  }
  void generate_save_cmp(L1::Register* dst, L1::Register* r, L1::CmpOperation* cmpOP, L1::Number* n) {
    outputFile << "\t\t" << "cmpq $" << std::to_string(n->get()) << ", %" << Architecture::to_string(r->get()) << std::endl;
    outputFile << "\t\tset" << Architecture::get_cmp_instr(cmpOP->get()) << " %" << Architecture::get_eight_bit(dst->get()) << std::endl;
    outputFile << "\t\tmovzbq %" << Architecture::get_eight_bit(dst->get()) << ", %" << Architecture::to_string(dst->get()) << std::endl;
    return;
  }
  void generate_save_cmp(L1::Register* dst, L1::Number* n, L1::CmpOperation* cmpOP, L1::Register* r) {
    generate_save_cmp(dst, r, cmpOP, n);
    return;
  }
  void generate_save_cmp(L1::Register* dst, L1::Number* n1, L1::CmpOperation* cmpOP, L1::Number* n2) {
    auto raw_op = cmpOP->get();
    bool comparison_result;
    switch (raw_op) {
      case Architecture::CompareOP::less_than:
        comparison_result = n1->get() < n2->get();
        break;
      case Architecture::CompareOP::equal:
        comparison_result = n1->get() == n2->get();
        break;
      case Architecture::CompareOP::less_than_or_equal:
        comparison_result = n1->get() <= n2->get();
        break;
      default:
        std::cerr << "ERROR in generate_save_cmp n1 cmp n2" << std::endl;
    }
    int to_store = (int)comparison_result;
    outputFile << "\t\tmovq $" << std::to_string(to_store) << ", %" << Architecture::to_string(dst->get()) << std::endl;
    return;
  }
  // cjump t cmp t label
  void generate_cjump(L1::Register* r1, L1::CmpOperation* cmpOP, L1::Register* r2, L1::Label* label) {
    outputFile << "\t\tcmpq %" << Architecture::to_string(r2->get()) << ", %" << Architecture::to_string(r1->get()) << std::endl;
    outputFile << "\t\tj" << Architecture::get_cmp_instr(cmpOP->get()) << " _" << label->get().substr(1) << std::endl;
    return;
  }
  void generate_cjump(L1::Number* n, L1::CmpOperation* cmpOP, L1::Register* r, L1::Label* label) {
    generate_cjump(r, cmpOP, n, label);
    return;
  }
  void generate_cjump(L1::Register* r, L1::CmpOperation* cmpOP, L1::Number* n, L1::Label* label) {
    outputFile << "\t\tcmpq $" << std::to_string(n->get()) << ", %" << Architecture::to_string(r->get()) << std::endl;
    outputFile << "\t\tj" << Architecture::get_cmp_instr(cmpOP->get()) << " _" << label->get().substr(1) << std::endl;
    return;
  }
  void generate_cjump(L1::Number* n1, L1::CmpOperation* cmpOP, L1::Number* n2, L1::Label* label) {
    auto raw_op = cmpOP->get();
    bool comparison_result;
    switch (raw_op) {
      case Architecture::CompareOP::less_than:
        comparison_result = n1->get() < n2->get();
        break;
      case Architecture::CompareOP::equal:
        comparison_result = n1->get() == n2->get();
        break;
      case Architecture::CompareOP::less_than_or_equal:
        comparison_result = n1->get() <= n2->get();
        break;
      default:
        std::cerr << "ERROR in generate_save_cmp n1 cmp n2" << std::endl;
    }
    if (comparison_result) {
      outputFile << "\t\tjmp _" << label->get().substr(1) << std::endl;
    }
    return;
  }
  // label
  void generate_label(L1::Label* label) {
    outputFile << "_" << label->get().substr(1) << ":" << std::endl;
    return;
  }
  // goto label
  void generate_goto(L1::Label* label) {
    outputFile << "\t\tjmp _" << label->get().substr(1) << std::endl;
    return;
  }
  // return
  void generate_return(L1::Number* num_locals) {
    outputFile << "\t\taddq $" << std::to_string(num_locals->get() * 8) << ", %rsp" << std::endl;
    outputFile << "\t\tretq" << std::endl;
    return;
  }
  // call u N
  void generate_call(L1::Register* r, L1::Number* n) {
    outputFile << "\t\tsubq $" << std::to_string(8 + (8 * std::max(0, ((int)n->get() - 6)))) << ", %rsp" << std::endl;
    outputFile << "\t\tjmp *%" << Architecture::to_string(r->get()) << std::endl;
    return;
  }
  void generate_call(L1::Label* label, L1::Number* n) {
    outputFile << "subq $" << std::to_string(8 + (8 * std::max(0, ((int)n->get() - 6)))) << ", %rsp" << std::endl;
    outputFile << "\t\tjmp _" << label->get().substr(1) << std::endl;
    return;
  }
  // call print 1
  void generate_call_print() {
    outputFile << "\t\tcall print" << std::endl;
    return;
  }
  // call input 0
  void generate_call_input() {
    outputFile << "\t\tcall input" << std::endl;
    return;
  }
  // call allocate 2
  void generate_call_allocate() {
    outputFile << "\t\tcall allocate" << std::endl;
    return;
  }
  // call tensor-error F
  void generate_call_tensorError(L1::Number* n) {
    auto arg = n->get();
    outputFile << "\t\tcall ";
    switch (arg) {
      case 1:
        outputFile << "array_tensor_error_null";
        break;
      case 3:
        outputFile << "array_error";
        break;
      case 4:
        outputFile << "tensor_error";
        break;
    }
    outputFile << std::endl;
    return;
  }
  // w ++, w --
  void generate_op(L1::Register* r, L1::Operation* op, L1::NullItem* blank, L1::NullItem* blank2, L1::NullItem* blank3) {
    outputFile << "\t\t" << std::get<0>(Architecture::get_op_instr(op->get())) << " %" << Architecture::to_string(r->get()) << std::endl;
    return;
  }
  // w @ w w E
  void generate_op(L1::Register* dst, L1::Operation* op, L1::Register* first, L1::Register* second, L1::Number* factor) {
    outputFile << "\t\tlea (%" << Architecture::to_string(first->get()) << ", %" << Architecture::to_string(second->get()) << ", " << std::to_string(factor->get()) << "), %" << Architecture::to_string(dst->get()) << std::endl;
    return;
  }

  void Assembly_visitor::visit(const L1::Instruction_return* i) const {
    auto instr_data = i->get();
    auto type_name = typeid(*instr_data).name();
    if (type_name != NumberName) {
      std::cerr << "ERROR in visit Instruction_return."<< std::endl;
    }
    L1::Number* n = (L1::Number*)instr_data;
    generate_return(n);
    return;
  }
  void Assembly_visitor::visit(const L1::Instruction_assignment* i) const {
    auto instr_data = i->get();
    auto type_name_1 = typeid(*instr_data[0]).name();
    auto type_name_2 = typeid(*instr_data[1]).name();
    if (type_name_1 == RegisterName && type_name_2 == NumberName) {
      L1::Register* r = (L1::Register*)instr_data[0];
      L1::Number* n = (L1::Number*)instr_data[1];
      generate_assignment(r, n);
    } else if (type_name_1 == RegisterName && type_name_2 == LabelName) {
      L1::Register* r = (L1::Register*)instr_data[0];
      L1::Label* label = (L1::Label*)instr_data[1];
      generate_assignment(r, label);
    } else if (type_name_1 == RegisterName && type_name_2 == RegisterName) {
      L1::Register* r1 = (L1::Register*)instr_data[0];
      L1::Register* r2 = (L1::Register*)instr_data[1];
      generate_assignment(r1, r2);
    } else if (type_name_1 == RegisterName && type_name_2 == MemoryLocationName) {
      L1::Register* r = (L1::Register*)instr_data[0];
      L1::MemoryLocation* m = (L1::MemoryLocation*)instr_data[1];
      generate_assignment(r, m);
    } else if (type_name_1 == MemoryLocationName && type_name_2 == NumberName) {
      L1::MemoryLocation* m = (L1::MemoryLocation*)instr_data[0];
      L1::Number* n = (L1::Number*)instr_data[1];
      generate_assignment(m, n);
    } else if (type_name_1 == MemoryLocationName && type_name_2 == LabelName) {
      L1::MemoryLocation* m = (L1::MemoryLocation*)instr_data[0];
      L1::Label* label = (L1::Label*)instr_data[1];
      generate_assignment(m, label);
    } else if (type_name_1 == MemoryLocationName && type_name_2 == RegisterName) {
      L1::MemoryLocation* m = (L1::MemoryLocation*)instr_data[0];
      L1::Register* r = (L1::Register*)instr_data[1];
      generate_assignment(m, r);
    } else {
      std::cerr << "ERROR in visit Instruction_assignment" << std::endl;
    }

    return;
  }
  void Assembly_visitor::visit(const L1::Instruction_operation* i) const {
    auto instr_data = i->get();
    auto type_name_1 = typeid(*instr_data[0]).name();
    auto type_name_2 = typeid(*instr_data[1]).name();
    auto type_name_3 = typeid(*instr_data[2]).name();
    auto type_name_4 = typeid(*instr_data[3]).name();
    auto type_name_5 = typeid(*instr_data[4]).name();
    if (type_name_1 == RegisterName && type_name_3 == RegisterName) {
      L1::Register* r1 = (L1::Register*)instr_data[0];
      L1::Operation* op = (L1::Operation*)instr_data[1];
      L1::Register* r2 = (L1::Register*)instr_data[2];
      L1::NullItem* empty1 = (L1::NullItem*)instr_data[3];
      L1::NullItem* empty2 = (L1::NullItem*)instr_data[4];
      generate_op(r1, op, r2, empty1, empty2);
    } else if (type_name_1 == RegisterName && type_name_3 == NumberName) {
      L1::Register* r = (L1::Register*)instr_data[0];
      L1::Operation* op = (L1::Operation*)instr_data[1];
      L1::Number* n = (L1::Number*)instr_data[2];
      L1::NullItem* empty1 = (L1::NullItem*)instr_data[3];
      L1::NullItem* empty2 = (L1::NullItem*)instr_data[4];
      generate_op(r, op, n, empty1, empty2);
    } else if (type_name_1 == MemoryLocationName && type_name_3 == RegisterName) {
      L1::MemoryLocation* m = (L1::MemoryLocation*)instr_data[0];
      L1::Operation* op = (L1::Operation*)instr_data[1];
      L1::Register* r = (L1::Register*)instr_data[2];
      L1::NullItem* empty1 = (L1::NullItem*)instr_data[3];
      L1::NullItem* empty2 = (L1::NullItem*)instr_data[4];
      generate_op(m, op, r, empty1, empty2);
    } else if (type_name_1 == MemoryLocationName && type_name_3 == NumberName) {
      L1::MemoryLocation* m = (L1::MemoryLocation*)instr_data[0];
      L1::Operation* op = (L1::Operation*)instr_data[1];
      L1::Number* n = (L1::Number*)instr_data[2];
      L1::NullItem* empty1 = (L1::NullItem*)instr_data[3];
      L1::NullItem* empty2 = (L1::NullItem*)instr_data[4];
      generate_op(m, op, n, empty1, empty2);
    } else if (type_name_1 == RegisterName && type_name_3 == MemoryLocationName) {
      L1::Register* r = (L1::Register*)instr_data[0];
      L1::Operation* op = (L1::Operation*)instr_data[1];
      L1::MemoryLocation* m = (L1::MemoryLocation*)instr_data[2];
      L1::NullItem* empty1 = (L1::NullItem*)instr_data[3];
      L1::NullItem* empty2 = (L1::NullItem*)instr_data[4];
      generate_op(r, op, m, empty1, empty2);
    } else if (type_name_5 != NullItemName) {
      L1::Register* dst = (L1::Register*)instr_data[0];
      L1::Operation* op = (L1::Operation*)instr_data[1];
      L1::Register* r1 = (L1::Register*)instr_data[2];
      L1::Register* r2 = (L1::Register*)instr_data[3];
      L1::Number* n = (L1::Number*)instr_data[4];
      generate_op(dst, op, r1, r2, n);
    } else {
      L1::Register* r = (L1::Register*)instr_data[0];
      L1::Operation* op = (L1::Operation*)instr_data[1];
      L1::NullItem* empty1 = (L1::NullItem*)instr_data[2];
      L1::NullItem* empty2 = (L1::NullItem*)instr_data[3];
      L1::NullItem* empty3 = (L1::NullItem*)instr_data[4];
      generate_op(r, op, empty1, empty2, empty3);
    } 
    return;
  }
  void Assembly_visitor::visit(const L1::Instruction_cjump* i) const {
    auto instr_data = i->get();
    generate_cjump(instr_data[0], instr_data[1], instr_data[2], instr_data[3]);
    return;
  }
  void Assembly_visitor::visit(const L1::Instruction_save_cmp* i) const {
    auto instr_data = i->get();
    generate_save_cmp(instr_data[0], instr_data[1], instr_data[2], instr_data[3]);
    return;
  }
  void Assembly_visitor::visit(const L1::Instruction_label* i) const {
    auto instr_data = i->get();
    generate_label(instr_data);
    return;
  }
  void Assembly_visitor::visit(const L1::Instruction_goto* i) const {
    auto instr_data = i->get();
    generate_goto(instr_data);
    return;
  }
  void Assembly_visitor::visit(const L1::Instruction_call* i) const {
    auto instr_data = i->get();
    generate_call(instr_data[0], instr_data[1]);
    return;
  }
  void Assembly_visitor::visit(const L1::Instruction_call_print* i) const {
    generate_call_print();
    return;
  }
  void Assembly_visitor::visit(const L1::Instruction_call_input* i) const {
    generate_call_input();
    return;
  }
  void Assembly_visitor::visit(const L1::Instruction_call_allocate* i) const {
    generate_call_allocate();
    return;
  }
  void Assembly_visitor::visit(const L1::Instruction_call_tensorError* i) const {
    generate_call_tensorError(i->get());
    return;
  }

  void Assembly_visitor::visit(const L1::Program* p) const {
    outputFile << "\t\t.text" << std::endl << "\t\t.globl go" << std::endl << "go:" << std::endl;
    save_calle_saved_registers();
    outputFile << "call " << p->entryPointLabel << std::endl;
    restore_calle_saved_registers();
    outputFile << "retq" << std::endl;
    for (auto f: p->functions) {
      f->accept(generator);
    }
    return;
  }

  void Assembly_visitor::visit(const L1::Function* fn) const{
    outputFile << "@" << fn->name.substr(1) << ":" << std::endl;
    for (auto i: fn->instructions) {
      i->accept(generator);
    }
    return;
  }

}
