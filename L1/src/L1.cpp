#include <L1.h>
#include <iostream>
#include <string>

// DO I NEED TO INCLUDE CONST/OVERRIDE KEYWORDS IN HERE??

namespace L1 {

  // NullItem
  NullItem::NullItem() {
    return;
  }

  std::string NullItem::to_string() const {
    return "\t\t\tNullItem\n";
  }

  // Number
  Number::Number(int64_t value) {
    value = value;
    return;
  }

  std::string Number::to_string() const {
    return "\t\t\tValue: " + std::to_string(value) + "\n";
  }

  int64_t Number::get() const {
    return value;
  }

  bool Number::operator == (const Number &other) const {
    return value == other.get();
  }

  // Label
  Label::Label(std::string value) {
    value = value;
    return;
  }

  std::string Label::to_string() const {
    return "\t\t\tValue: " + value + "\n";
  }

  std::string Label::get() const {
    return value;
  }

  bool Label::operator == (const Label &other) const {
    return value == other.get();
  }

  // Register
  Register::Register(Architecture::RegisterID r) {
    ID = r;
    return;
  }

  std::string Register::to_string() const {
    return "\t\t\tRegister: " + Architecture::to_string(ID) + "\n";
  }

  Architecture::RegisterID Register::get() const {
    return ID;
  }

  bool Register::operator == (const Register &other) const {
    return ID == other.get();
  }

  // MemoryLocation
  MemoryLocation::MemoryLocation(Item* base, Item* off) {
    base_register = base;
    off = offset;
    return;
  }

  std::string MemoryLocation::to_string() const {
    return "\t\t\tMemoryLocation:\n\t" + base_register->to_string() + "\t" + offset->to_string();
  }

  std::pair<Item*, Item*> MemoryLocation::get() const {
    return std::make_pair(base_register, offset);
  }

  bool MemoryLocation::operator == (const MemoryLocation &other) const {
    return this->get() == other.get();
  }

  // Operation
  Operation::Operation(Architecture::OP op) {
    OP = op;
    return;
  }

  std::string Operation::to_string() const {
    return "\t\t\tOP: " + Architecture::to_string(OP) + "\n";
  }

  Architecture::OP Operation::get() const {
    return OP;
  }

  bool Operation::operator == (const Operation &other) const {
    return OP == other.get();
  }

  // CmpOperation
  CmpOperation::CmpOperation(Architecture::CompareOP op) {
    cmpOP = op;
    return;
  }

  std::string CmpOperation::to_string() const {
    return "\t\t\tCompareOP: " + Architecture::to_string(cmpOP) + "\n";
  }

  Architecture::CompareOP CmpOperation::get() const {
    return cmpOP;
  }

  bool CmpOperation::operator == (const CmpOperation &other) const {
    return cmpOP == other.get();
  }

  std::string InstructionOffset = "\t\t";

  // Instruction_return
  Instruction_return::Instruction_return() {
    return;
  }

  std::string Instruction_return::to_string() const {
    return InstructionOffset + "Instruction_return\n";
  }

  void Instruction_return::accept(Visitor* v) const {
    v->visit(this);
    return;
  }

  // Instruction_assignment
  Instruction_assignment::Instruction_assignment(Item* dst, Item* src) {
    src = src;
    dst = dst;
  }
  std::string Instruction_assignment::to_string() const {
    return InstructionOffset + "Instruction_assignment:\n" + src->to_string() + dst->to_string();
  }

  void Instruction_assignment::accept(Visitor* v) const {
    v->visit(this);
    return;
  }

  // Instruction_operation
  Instruction_operation::Instruction_operation(Item* left, Item* op, Item* right, Item* lea_reg, Item* factor) {
    OP = op;
    left = left;
    right = right;
    lea_reg = lea_reg;
    factor = factor;
  }

  std::string Instruction_operation::to_string() const {
    return InstructionOffset + "Instruction_operation:\n" + left->to_string() + OP->to_string() + right->to_string() + lea_reg->to_string() + factor->to_string();
  }

  void Instruction_operation::accept(Visitor* v) const {
    v->visit(this);
    return;
  }

  // Instruction_cjump
  Instruction_cjump::Instruction_cjump(Item* left, Item* cmpOP, Item* right, Item* label) {
    OP = cmpOP;
    left = left;
    right = right;
    label = label;
  }

  std::string Instruction_cjump::to_string() const {
    return InstructionOffset + "Instruction_cjump:\n" + left->to_string() + OP->to_string() + right->to_string() + label->to_string();
  }

  void Instruction_cjump::accept(Visitor* v) const {
    v->visit(this);
    return;
  }

  // Instruction_save_cmp
  Instruction_save_cmp::Instruction_save_cmp(Item* dst, Item* left, Item* cmpOP, Item* right) {
    dst = dst;
    OP = cmpOP;
    left = left;
    right = right;
  }

  std::string Instruction_save_cmp::to_string() const {
    return InstructionOffset + "Instruction_save_cmp:\n" + dst->to_string() + OP->to_string() + left->to_string() + right->to_string();
  }

  void Instruction_save_cmp::accept(Visitor* v) const {
    v->visit(this);
    return;
  }

  // Instruction_label
  Instruction_label::Instruction_label(Item* label) {
    label = label;
    return;
  }

  std::string Instruction_label::to_string() const {
    return InstructionOffset + "Instruction_label:\n" + label->to_string();
  }

  void Instruction_label::accept(Visitor* v) const {
    v->visit(this);
    return;
  }

  // Instruction_goto
  Instruction_goto::Instruction_goto(Item* label) {
    label = label;
    return;
  }

  std::string Instruction_goto::to_string() const {
    return InstructionOffset + "Instruction_goto:\n" + label->to_string();
  }

  void Instruction_goto::accept(Visitor* v) const {
    v->visit(this);
    return;
  }

  // Instruction_call
  Instruction_call::Instruction_call(Item* fn, Item* arg) {
    fn = fn;
    arg = arg;
    return;
  }

  std::string Instruction_call::to_string() const {
    return InstructionOffset + "Instruction_call:\n" + fn->to_string() + arg->to_string();
  }

  void Instruction_call::accept(Visitor* v) const {
    v->visit(this);
    return;
  }

  // Instruction_call_print
  Instruction_call_print::Instruction_call_print() {
    return;
  }

  std::string Instruction_call_print::to_string() const {
    return InstructionOffset + "Instruction_call_print:\n";
  }

  void Instruction_call_print::accept(Visitor* v) const {
    v->visit(this);
    return;
  }

  // Instruction_call_input
  Instruction_call_input::Instruction_call_input() {
    return;
  }

  std::string Instruction_call_input::to_string() const {
    return InstructionOffset + "Instruction_call_input:\n";
  }

  void Instruction_call_input::accept(Visitor* v) const {
    v->visit(this);
    return;
  }

  // Instruction_call_allocate
  Instruction_call_allocate::Instruction_call_allocate() {
    return;
  }

  std::string Instruction_call_allocate::to_string() const {
    return InstructionOffset + "Instruction_call_allocate:\n";
  }

  void Instruction_call_allocate::accept(Visitor* v) const {
    v->visit(this);
    return;
  }

  // Instruction_call_tensorError
  Instruction_call_tensorError::Instruction_call_tensorError(Item* arg) {
    arg = arg;
    return;
  }

  std::string Instruction_call_tensorError::to_string() const {
    return InstructionOffset + "Instruction_call_tensorError:\n";
  }

  void Instruction_call_tensorError::accept(Visitor* v) const {
    v->visit(this);
    return;
  }

  void Function::to_string() const {
    std::cout << "This is a function with name " << name << std::endl;
    std::cout << "and num_arguments " << std::to_string(arguments) << std::endl;
    std::cout << "and num_locals " << std::to_string(locals) << std::endl;
    std::cout << "and its instructions are: ";
    for (auto step : instructions) {
      std::cout << step->to_string();
    }
  }

  void Function::accept(Visitor* v) const {
    v->visit(this);
    return;
  }

  void Program::to_string() const {
    std::cout << "This is a Program with entryPointLabel " << entryPointLabel << std::endl;
    std::cout << "and its functions are: ";
    for (auto fn : functions) {
      fn->to_string();
    }
  }
  
  void Program::accept(Visitor* v) const {
    v->visit(this);
    return;
  }

}
