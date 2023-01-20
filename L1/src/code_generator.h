#pragma once

#include <L1.h>

#include <fstream>

namespace Generator{

  extern std::ofstream outputFile;

  void generate_code(Program p);

  // return
  void generate_return() const;

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
  // w aop t
  void generate_op(L1::Register* r1, L1::Register* r2) const;
  // ...
  // w <- t cmp t
  void generate_save_cmp(Architecture::CompareOP, L1::Register r1, L1::Regsiter r2) const;
  void generate_save_cmp(Architecture::CompareOP, L1::Register r, L1::Number n) const;
  void generate_save_cmp(Architecture::CompareOP, L1::Number n, L1::Register r) const;
  void generate_save_cmp(Architecture::CompareOP, L1::Number n1, L1::Number n2) const;

  void save_calle_saved_registers(std::ofstream file);
  
  void restore_calle_saved_registers(std::ofstream file);

  class Assembly_visitor : public L1::Visitor {
    public:
      void visit(const Instruction_return* i) const;
      void visit(const Instruction_assignment* i) const;
      void visit(const Instruction_operation* i) const;
      void visit(const Instruction_cjump* i) const;
      void visit(const Instruction_save_cmp* i) const;
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
