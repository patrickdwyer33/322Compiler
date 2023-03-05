#include <L2.h>
#include <iostream>
#include <string>

namespace L2 {

  // NullItem
  NullItem::NullItem() {
    return;
  }

  std::string NullItem::to_string() {
    return "";
  }

  // Number
  Number::Number(int64_t value) {
    this->value = value;
    return;
  }

  std::string Number::to_string() {
    return std::to_string(this->value);
  }

  int64_t Number::get() const {
    return this->value;
  }

  // Label
  Label::Label(std::string value) {
    this->value = value;
    return;
  }

  std::string Label::to_string() {
    return this->value;
  }

  std::string Label::get() const {
    return this->value;
  }


  // Variable
  Variable::Variable(std::string s) {
    this->s = s;
    return;
  }

  std::string Variable::to_string() {
    return this->s;
  }

  std::string Variable::get() const {
    return this->s;
  }

  // Register
 //construction in header file

  Architecture::RegisterID Register::get_reg() const {
    return this->ID;
  }

  // StackArg
  StackArg::StackArg(Number* offset) {
    this->offset = offset;
    return;
  }

  std::string StackArg::to_string() {
    return "stack-arg " + this->offset->to_string();
  }

  Number* StackArg::get() const {
    return this->offset;
  }

  // MemoryLocation
  MemoryLocation::MemoryLocation(Variable* base, Number* offset) {
    this->base_var = base;
    this->offset = offset;
    return;
  }

  std::string MemoryLocation::to_string() {
    return "mem " + this->base_var->to_string() + " " + this->offset->to_string();
  }

  std::vector<Item*> MemoryLocation::get() const {
    std::vector<Item*> vec = {this->base_var, this->offset};
    return vec;
  }

  // Operation
  Operation::Operation(Architecture::OP op) {
    this->OP = op;
    return;
  }

  std::string Operation::to_string() {
    return Architecture::to_string(this->OP);
  }

  Architecture::OP Operation::get() const {
    return this->OP;
  }

  // CmpOperation
  CmpOperation::CmpOperation(Architecture::CompareOP op) {
    this->cmpOP = op;
    return;
  }

  std::string CmpOperation::to_string() {
    return Architecture::to_string(this->cmpOP);
  }

  Architecture::CompareOP CmpOperation::get() const {
    return this->cmpOP;
  }

  std::string InstructionOffset = "\t\t";

  // Instruction_return
  Instruction_return::Instruction_return(Item* num_locals, Item* num_args) {
    this->num_locals = num_locals;
    this->num_args = num_args;
    return;
  }

  std::string Instruction_return::to_string() {
    return InstructionOffset + "Instruction_return\n";
  }

  void Instruction_return::accept(Visitor* v) {
    v->visit(this);
    return;
  }

  std::vector<Item*> Instruction_return::get() const {
    std::vector<Item*> vec = {this->num_locals, this->num_args};
    return vec;
  }

  // Instruction_assignment
  Instruction_assignment::Instruction_assignment(Item* dst, Item* src) {
    this->src = src;
    this->dst = dst;
  }
  std::string Instruction_assignment::to_string() {
    return InstructionOffset + "Instruction_assignment:\n" + this->src->to_string() + this->dst->to_string();
  }

  void Instruction_assignment::accept(Visitor* v) {
    v->visit(this);
    return;
  }

  std::vector<Item*> Instruction_assignment::get() const {
    std::vector<Item*> vec = {this->dst, this->src};
    return vec;
  }

  // Instruction_operation
  Instruction_operation::Instruction_operation(Item* left, Item* op, Item* right, Item* lea_reg, Item* factor) {
    this->OP = op;
    this->left = left;
    this->right = right;
    this->lea_reg = lea_reg;
    this->factor = factor;
  }

  std::string Instruction_operation::to_string() {
    return InstructionOffset + "Instruction_operation:\n" + this->left->to_string() + this->OP->to_string() + this->right->to_string() + this->lea_reg->to_string() + this->factor->to_string();
  }

  void Instruction_operation::accept(Visitor* v) {
    v->visit(this);
    return;
  }

  std::vector<Item*> Instruction_operation::get() const {
    std::vector<Item*> vec = {left, OP, right, lea_reg, factor};
    return vec;
  }

  // Instruction_cjump
  Instruction_cjump::Instruction_cjump(Item* left, Item* cmpOP, Item* right, Item* label) {
    this->OP = cmpOP;
    this->left = left;
    this->right = right;
    this->label = label;
  }

  std::string Instruction_cjump::to_string() {
    return InstructionOffset + "Instruction_cjump:\n" + this->left->to_string() + this->OP->to_string() + this->right->to_string() + this->label->to_string();
  }

  void Instruction_cjump::accept(Visitor* v) {
    v->visit(this);
    return;
  }

  std::vector<Item*> Instruction_cjump::get() const {
    std::vector<Item*> vec = {this->left, this->OP, this->right, this->label};
    return vec;
  }

  // Instruction_save_cmp
  Instruction_save_cmp::Instruction_save_cmp(Item* dst, Item* left, Item* cmpOP, Item* right) {
    this->dst = dst;
    this->OP = cmpOP;
    this->left = left;
    this->right = right;
  }

  std::string Instruction_save_cmp::to_string() {
    return InstructionOffset + "Instruction_save_cmp:\n" + this->dst->to_string() + this->OP->to_string() + this->left->to_string() + this->right->to_string();
  }

  void Instruction_save_cmp::accept(Visitor* v) {
    v->visit(this);
    return;
  }

  std::vector<Item*> Instruction_save_cmp::get() const {
    std::vector<Item*> vec = {this->dst, this->left, this->OP, this->right};
    return vec;
  }

  // Instruction_label
  Instruction_label::Instruction_label(Item* label) {
    this->label = label;
    return;
  }

  std::string Instruction_label::to_string() {
    return InstructionOffset + "Instruction_label:\n" + this->label->to_string();
  }

  void Instruction_label::accept(Visitor* v) {
    v->visit(this);
    return;
  }

  Item* Instruction_label::get() const {
    return this->label;
  }

  // Instruction_goto
  Instruction_goto::Instruction_goto(Item* label) {
    this->label = label;
    return;
  }

  std::string Instruction_goto::to_string() {
    return InstructionOffset + "Instruction_goto:\n" + this->label->to_string();
  }

  void Instruction_goto::accept(Visitor* v) {
    v->visit(this);
    return;
  }

  Item* Instruction_goto::get() const {
    return this->label;
  }

  // Instruction_call
  Instruction_call::Instruction_call(Item* fn, Item* arg) {
    this->fn = fn;
    this->arg = arg;
    return;
  }

  std::string Instruction_call::to_string() {
    return InstructionOffset + "Instruction_call:\n" + this->fn->to_string() + this->arg->to_string();
  }

  void Instruction_call::accept(Visitor* v) {
    v->visit(this);
    return;
  }

  std::vector<Item*> Instruction_call::get() const {
    std::vector<Item*> vec = {this->fn, this->arg};
    return vec;
  }

  // Instruction_call_print
  Instruction_call_print::Instruction_call_print() {
    return;
  }

  std::string Instruction_call_print::to_string() {
    return InstructionOffset + "Instruction_call_print:\n";
  }

  void Instruction_call_print::accept(Visitor* v) {
    v->visit(this);
    return;
  }

  // Instruction_call_input
  Instruction_call_input::Instruction_call_input() {
    return;
  }

  std::string Instruction_call_input::to_string() {
    return InstructionOffset + "Instruction_call_input:\n";
  }

  void Instruction_call_input::accept(Visitor* v) {
    v->visit(this);
    return;
  }

  // Instruction_call_allocate
  Instruction_call_allocate::Instruction_call_allocate() {
    return;
  }

  std::string Instruction_call_allocate::to_string() {
    return InstructionOffset + "Instruction_call_allocate:\n";
  }

  void Instruction_call_allocate::accept(Visitor* v) {
    v->visit(this);
    return;
  }

  // Instruction_call_tensorError
  Instruction_call_tensorError::Instruction_call_tensorError(Item* arg) {
    this->arg = arg;
    return;
  }

  std::string Instruction_call_tensorError::to_string() {
    return InstructionOffset + "Instruction_call_tensorError:\n";
  }

  void Instruction_call_tensorError::accept(Visitor* v) {
    v->visit(this);
    return;
  }

  Item* Instruction_call_tensorError::get() const {
    return this->arg;
  }

  fence_node::fence_node(L2::Variable var) {
    this->var = &var;
    this->num_neighbors = 0;
    this->color = 15;
  }

  void Function::to_string() {
    std::cout << "This is a function with name " << this->name << std::endl;
    std::cout << "and num_arguments " << std::to_string(this->arguments) << std::endl;
    std::cout << "and num_locals " << std::to_string(this->locals) << std::endl;
    std::cout << "and its instructions are: ";
    for (auto step : this->instructions) {
      std::cout << step->to_string();
    }
  }

  void Function::accept(Visitor* v) {
    v->visit(this);
    return;
  }

  void Program::to_string() {
    std::cout << "This is a Program with entryPointLabel " << this->entryPointLabel << std::endl;
    std::cout << "and its functions are: ";
    for (auto fn : this->functions) {
      fn->to_string();
    }
  }
  
  void Program::accept(Visitor* v) {
    v->visit(this);
    return;
  }

}
