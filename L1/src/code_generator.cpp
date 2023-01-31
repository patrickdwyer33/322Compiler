#include <string>
#include <iostream>
#include <fstream>
#include <L1.h>

#include <code_generator.h>

using namespace std;

namespace Generator{

  std::ofstream outputFile;

  void generate_code(L1::Program p) {

    /* 
     * Open the output file.
     */ 
    outputFile.open("prog.S");
   
    /* 
     * Generate target code
     */
    Generator::Assembly_visitor* generator;
    generator->visit(&p);

    /* 
     * Close the output file.
     */ 
    outputFile.close();
   
    return;
  }

  void save_calle_saved_registers(std::ofstream file) {
    file << "  pushq %rbx" << std::endl;
    file << "  pushq %rbp" << std::endl;
    file << "  pushq %r12" << std::endl;
    file << "  pushq %r13" << std::endl;
    file << "  pushq %r14" << std::endl;
    file << "  pushq %r15" << std::endl;
    return;
  }

  void restore_calle_saved_registers(std::ofstream file) {
    file << "  popq %r15" << std::endl;
    file << "  popq %r14" << std::endl;
    file << "  popq %r13" << std::endl;
    file << "  popq %r12" << std::endl;
    file << "  popq %rbp" << std::endl;
    file << "  popq %rbx" << std::endl;
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
  void generate_assignment(L1::Register* r, L1::MemoryLocation mem) {
    auto mem_info = mem->get();
    outputFile << "\t\tmovq " << std::to_string(mem_info[1]->get())<< "(%" << Architecture::to_string(mem_info[0]->get()) << ") %" << Architecture::to_string(r->get()) << std::endl;
    return;
  }
  // mem x M <- s
  void generate_assignment(L1::MemoryLocation mem, L1::Number* n) {
    auto mem_info = mem->get();
    outputFile << "\t\tmovq $" << Architecture::to_string(n->get()) << ", " << std::to_string(mem_info[1]->get())<< "(%" << Architecture::to_string(mem_info[0]->get()) << ")" << std::endl;
    return;
  }
  void generate_assignment(L1::MemoryLocation mem, L1::Label* label) {
    auto mem_info = mem->get();
    outputFile << "\t\tmovq $" << label->get() << ", " << std::to_string(mem_info[1]->get())<< "(%" << Architecture::to_string(mem_info[0]->get()) << ")" << std::endl;
    return;
  }
  void generate_assignment(L1::MemoryLocation mem, L1::Register* r) {
    auto mem_info = mem->get();
    outputFile << "\t\tmovq %" << Architecture::to_string(r->get()) << ", " << std::to_string(mem_info[1]->get())<< "(%" << Architecture::to_string(mem_info[0]->get()) << ")" << std::endl;
    return;
  }
  // w aop t, w sop sx, w sop N
  void generate_op(L1::Register* r1, L1::Operation* op, L1::Register* r2, L1::NullItem* blank, L1::NullItem* blank2) {
    auto raw_op = op->get();
    std::string instruction;
    switch (raw_op) {
      case Architecture::OP::plus_equals:
        instruction = "addq"
        break;
      case Architecture::OP::minus_equals:
        instruction = "subq"
        break;
      case Architecture::OP::multiply_equals:
        instruction = "imulq"
        break;
      case Architecture::OP::and_equals:
        instruction = "andq"
        break;
      default:
        std::sterr << "ERROR in generate_op r1 <- r2" << std::endl;
    }
    outputFile << "\t\t" << instruction << " %" << Architecture::to_string(r2->get()) << ", %" << Architecture::to_string(r1->get()) << std::endl;
    return;
  }
  void generate_op(L1::Register* r, L1::Operation* op, L1::Number* n, L1::NullItem* blank, L1::NullItem* blank2) {
    auto raw_op = op->get();
    std::string instruction;
    switch (raw_op) {
      case Architecture::OP::plus_equals:
        instruction = "addq"
        break;
      case Architecture::OP::minus_equals:
        instruction = "subq"
        break;
      case Architecture::OP::multiply_equals:
        instruction = "imulq"
        break;
      case Architecture::OP::and_equals:
        instruction = "andq"
        break;
      default:
        std::sterr << "ERROR in generate_op r1 <- r2" << std::endl;
    }
    outputFile << "\t\t" << instruction << " $" << Architecture::to_string(n->get()) << ", %" << Architecture::to_string(r->get()) << std::endl;
    return;
  }
  // mem x M += t, mem x M -= t
  void generate_op(L1::MemoryLocation* mem, L1::Operation* op, L1::Register* r, L1::NullItem* blank, L1::NullItem* blank2) {
    auto raw_op = op->get();
    std::string instruction;
    switch (raw_op) {
      case Architecture::OP::plus_equals:
        instruction = "addq"
        break;
      case Architecture::OP::minus_equals:
        instruction = "subq"
        break;
      case Architecture::OP::multiply_equals:
        instruction = "imulq"
        break;
      case Architecture::OP::and_equals:
        instruction = "andq"
        break;
      default:
        std::sterr << "ERROR in generate_op r1 <- r2" << std::endl;
    }
    auto mem_info = mem->get();
    outputFile << "\t\t" << instruction << " %" << Architecture::to_string(r->get()) << ", " << std::to_string(mem_info[1]->get())<< "(%" << Architecture::to_string(mem_info[0]->get()) << ")" << std::endl;
    return;
  }
  void generate_op(L1::MemoryLocation* mem, L1::Operation* op, L1::Number* n, L1::NullItem* blank, L1::NullItem* blank2) {
    auto raw_op = op->get();
    std::string instruction;
    switch (raw_op) {
      case Architecture::OP::plus_equals:
        instruction = "addq"
        break;
      case Architecture::OP::minus_equals:
        instruction = "subq"
        break;
      case Architecture::OP::multiply_equals:
        instruction = "imulq"
        break;
      case Architecture::OP::and_equals:
        instruction = "andq"
        break;
      default:
        std::sterr << "ERROR in generate_op r1 <- r2" << std::endl;
    }
    auto mem_info = mem->get();
    outputFile << "\t\t" << instruction << " $" << Architecture::to_string(n->get()) << ", " << std::to_string(mem_info[1]->get())<< "(%" << Architecture::to_string(mem_info[0]->get()) << ")" << std::endl;
    return;
  }
  // w += mem x M, w -= mem x M
  void generate_op(L1::Register* r, L1::Operation* op, L1::MemoryLocation mem, L1::NullItem blank, L1::NullItem* blank2);
  // w <- t cmp t
  void generate_save_cmp(L1::Register* dst, L1::Register* r1, L1::CmpOperation* cmpOP, L1::Register* r2);
  void generate_save_cmp(L1::Register* dst, L1::Register* r, L1::CmpOperation* cmpOP, L1::Number* n);
  void generate_save_cmp(L1::Register* dst, L1::Number* n, L1::CmpOperation* cmpOP, L1::Register* r);
  void generate_save_cmp(L1::Register* dst, L1::Number* n1, L1::CmpOperation* cmpOP, L1::Number* n2);
  // cjump t cmp t label
  void generate_cjump(L1::Register* r1, L1::CmpOperation* cmpOP, L1::Register* r2, L1::Label* label);
  void generate_cjump(L1::Number* n, L1::CmpOperation* cmpOP, L1::Register* r, L1::Label* label);
  void generate_cjump(L1::Register* r, L1::CmpOperation* cmpOP, L1::Number* n, L1::Label* label);
  void generate_cjump(L1::Number* n1, L1::CmpOperation* cmpOP, L1::Number* n2, L1::Label* label);
  // label
  void generate_label(L1::Label* label);
  // goto label
  void generate_goto(L1::Label* label);
  // return
  void generate_return();
  // call u N
  void generate_call(L1::Register* r, L1::Number* n);
  void generate_call(L1::Label* r, L1::Number* n);
  // call print 1
  void generate_call_print();
  // call input 0
  void generate_call_input();
  // call allocate 2
  void generate_call_allocate();
  // call tensor-error F
  void generate_call_tensorError(L1::Number* n);
  // w ++, w --
  void generate_op(L1::Register* r, Architecture::OP op, L1::NullItem* blank, L1::NullItem* blank2, L1::NullItem* blank3);
  // w @ w w E
  void generate_op(L1::Register* dst, Architecture::OP op, L1::Register* first, L1::Register* second, L1::Number* factor);
}
