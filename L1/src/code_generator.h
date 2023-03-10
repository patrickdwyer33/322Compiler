#pragma once

#include <L1.h>

#include <fstream>

namespace L1 {

  //extern std::ofstream outputFile;

  void generate_code(L1::Program &p);

  void save_calle_saved_registers();
  
  void restore_calle_saved_registers();

  // w <- s
  void generate_assignment(L1::Register* r, L1::Number* n);
  void generate_assignment(L1::Register* r, L1::Label* label);
  void generate_assignment(L1::Register* r1, L1::Register* r2);
  // w <- mem x M
  void generate_assignment(L1::Register* r, L1::MemoryLocation mem);
  // mem x M <- s
  void generate_assignment(L1::MemoryLocation mem, L1::Number* n);
  void generate_assignment(L1::MemoryLocation mem, L1::Label* label);
  void generate_assignment(L1::MemoryLocation mem, L1::Register* r);
  // w aop t, w sop sx, w sop N
  void generate_op(L1::Register* r1, L1::Operation* op, L1::Register* r2, L1::NullItem* blank, L1::NullItem* blank2);
  void generate_op(L1::Register* r, L1::Operation* op, L1::Number* n, L1::NullItem* blank, L1::NullItem* blank2);
  // mem x M += t, mem x M -= t
  void generate_op(L1::MemoryLocation* mem, L1::Operation* op, L1::Register* r, L1::NullItem* blank, L1::NullItem* blank2);
  void generate_op(L1::MemoryLocation* mem, L1::Operation* op, L1::Number* n, L1::NullItem* blank, L1::NullItem* blank2);
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
  void generate_return(L1::Number* num_locals, L1::Number* num_args);
  // call u N
  void generate_call(L1::Register* r, L1::Number* n);
  void generate_call(L1::Label* label, L1::Number* n);
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

  class Assembly_visitor : public L1::Visitor {
    public:
      void visit(L1::Instruction_return* i);
      void visit(L1::Instruction_assignment* i);
      void visit(L1::Instruction_operation* i);
      void visit(L1::Instruction_cjump* i);
      void visit(L1::Instruction_save_cmp* i);
      void visit(L1::Instruction_label* i);
      void visit(L1::Instruction_goto* i);
      void visit(L1::Instruction_call* i);
      void visit(L1::Instruction_call_print* i);
      void visit(L1::Instruction_call_input* i);
      void visit(L1::Instruction_call_allocate* i);
      void visit(L1::Instruction_call_tensorError* i);
      void visit(L1::Function* fn);
      void visit(L1::Program* p);
  };

}
