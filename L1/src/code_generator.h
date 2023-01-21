#pragma once

#include <L1.h>

#include <fstream>

namespace Generator{

  extern std::ofstream outputFile;

  void generate_code(Program p);

  // w <- s
  void generate_assignment(L1::Register* r, L1::Number* n) const;
  void generate_assignment(L1::Register* r, L1::Label* label) const;
  void generate_assignment(L1::Register* r1, L1::Register* r2) const;
  // w <- mem x M
  void generate_assignment(L1::Register* r, L1::MemoryLocation mem) const;
  // mem x M <- s
  void generate_assignment(L1::MemoryLocation mem, L1::Number* n) const;
  void generate_assignment(L1::MemoryLocation mem, L1::Label* label) const;
  void generate_assignment(L1::MemoryLocation mem, L1::Register* r) const;
  // w aop t, w sop sx, w sop N
  void generate_op(L1::Register* r1, Architecture::OP op, L1::Register* r2, L1::NullItem blank, L1::NullItem blank) const;
  void generate_op(L1::Register* r, Architecture::OP op, Number* n, L1::NullItem blank, L1::NullItem blank) const;
  // mem x M += t, mem x M -= t
  void generate_op(L1::MemoryLocation* mem, Architecture::OP op, L1::Register* r, L1::NullItem blank, L1::NullItem blank) const;
  void generate_op(L1::MemoryLocation* mem, Architecture::OP op, L1::Number* r, L1::NullItem blank, L1::NullItem blank) const;
  // w += mem x M, w -= mem x M
  void generate_op(L1::Register* r, Architecture::OP op, L1::MemoryLocation mem, L1::NullItem blank, L1::NullItem blank) const;
  // w <- t cmp t
  void generate_save_cmp(L1::Register* dst, L1::Register* r1, Architecture::CompareOP cmpOP, L1::Regsiter* r2) const;
  void generate_save_cmp(L1::Register* dst, L1::Register* r, Architecture::CompareOP cmpOP, L1::Number* n) const;
  void generate_save_cmp(L1::Register* dst, L1::Number* n, Architecture::CompareOP cmpOP, L1::Register* r) const;
  void generate_save_cmp(L1::Register* dst, L1::Number* n1, Architecture::CompareOP cmpOP, L1::Number* n2) const;
  // cjump t cmp t label
  void generate_cjump(L1::Register* r1, Architecture::CompareOP cmpOP, L1::Register* r2, L1::Label* label) const;
  void generate_cjump(L1::Number* n, Architecture::CompareOP cmpOP, L1::Register* r, L1::Label* label) const;
  void generate_cjump(L1::Register* r, Architecture::CompareOP cmpOP, L1::Number* n, L1::Label* label) const;
  void generate_cjump(L1::Number* n1, Architecture::CompareOP cmpOP, L1::Number* n2, L1::Label* label) const;
  // label
  void generate_label(L1::Label* label) const;
  // goto label
  void generate_goto(L1::Label* label) const;
  // return
  void generate_return() const;
  // call u N
  void generate_call(L1::Register* r, L1::Number* n) const;
  void generate_call(L1::Label* r, L1::Number* n) const;
  // call print 1
  void generate_call_print() const;
  // call input 0
  void generate_call_input() const;
  // call allocate 2
  void generate_call_allocate() const;
  // call tensor-error F
  void generate_call_tensorError(L1::Number* n) const;
  // w ++, w --
  void generate_op(L1::Register* r, Architecture::OP op, L1::NullItem blank, L1::NullItem blank, L1::NullItem blank) const;
  // w @ w w E
  void generate_op(L1::Register* dst, Architecture::OP op, L1::Register* first, L1::Register* second, L1::Number* factor) const;

  void save_calle_saved_registers(std::ofstream file);
  
  void restore_calle_saved_registers(std::ofstream file);

  class Assembly_visitor : public L1::Visitor {
    public:
      void visit(const Instruction_return* i) const;
      void visit(const Instruction_assignment* i) const;
      void visit(const Instruction_operation* i) const;
      void visit(const Instruction_cjump* i) const;
      void visit(const Instruction_save_cmp* i) const;
      void visit(const Instruction_label* i) const;
      void visit(const Instruction_goto* i) const;
      void visit(const Instruction_call* i) const;
      void visit(const Instruction_call_print* i) const;
      void visit(const Instruction_call_input* i) const;
      void visit(const Instruction_call_allocate* i) const;
      void visit(const Instruction_call_tensorError* i) const;
      void visit(const Function* fn) const;
      void visit(const Program* p) const;
  }



}
