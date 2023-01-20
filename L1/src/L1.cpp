#include "L1.h"
#include <iostream>

namespace L1 {

  Item::Print() {
    std::cout << "This is an empty Item." << std::endl;
  }

  Number::Number(int64_t value) {
    Value = value;
    return;
  }

  Number::Print() {
    std::cout << "This is a Number with value " << std::to_string(Value) << std::endl;
  }

  Label::Label(string value) {
    Value = value;
    return;
  }

  Label::Print() {
    std::cout << "This is a Label with value " << Value << std::endl;
  }

  Register::Register(RegisterID r) {
    ID = r;
    return;
  }

  Register::Print() {
    std::cout << "This is a Register with RegisterID " << RegisterID << std::endl;
  }

  Mem_access::Mem_access(Item* reg, Item* offset) {
    Base_register = reg;
    Offset = offset
    return;
  }

  Mem_access::Print() {
    std::cout << "This is a Mem_access with Base_register " << RegisterID << std::endl;
    std::cout << "and Offset " << Offset << std::endl;
  }

  Operation::Operation(OperationID op, std::vector<Item*> args, int64_t num_args){
    ID = op;
    Args = args;
    Num_Args = num_args;
    return;
  }

  Operation::Print() {
    std::cout << "This is an Operation with OperationID " << ID << std::endl;
    std::cout << " and Args [ ";
    for (auto obj : Args) {
      std::cout << " " << obj.Print();
    }
    std::cout << "] and Num_Args " << Num_Args << std::endl;
  }

  Library_function::Library_function(LibraryCall fn) 
    : Fn { fn } {
    return;
  }

  Library_function::Print() {
    std::cout << "This is a Library_function with LibraryCall " << Fn << std::endl;
  }

  Instruction::Print() {
    std::cout << "This is an empty Instruction" << std::endl;
  }

  Instruction_ret::Print() {
    std::cout << "This is a Return Instruction" << std::endl;
  }

  Instruction_assignment::Instruction_assignment(Item* dst, Item* src)
    : s { src },
      d { dst } {
    return;
  }

  Instruction_assignment::Print() {
    std::cout << "This is an Assignment Instruction with Src: ";
    Src.Print();
    std::cout << "and Dst: ";
    Dst.Print();
  }

  Instruction_operation::Instruction_operation(Item* op)
    : Op { op } {
    return;
  }

  Instruction_operation::Print() {
    std::cout << "This is an Operation Instruction with Op: ";
    Op.Print();
  }

  Instruction_cjump::Instruction_cjump(Item* op, Item* label)
    : Op { op },
      Label { label } {
    return;
  }

  Instruction_cjump::Print() {
    std::cout << "This is a cJump Instruction with Op: ";
    Op.Print();
    std::cout << "and Label: ";
    Label.Print();
  }

  Instruction_goto::Instruction_goto(Item* label)
    : Label { label } {
    return;
  }

  Instruction_goto::Print() {
    std::cout << "This is a Goto Instruction with Label: ";
    Label.Print();
  }

  Instruction_call::Instruction_call(Item* fn, Item* arg)
    : Fn { fn },
      Arg { arg } {
    return;
  }

  Instruction_call::Print() {
    std::cout << "This is a Call Instruction with Fn: ";
    Fn.Print();
    std::cout << "and Arg: ";
    Arg.Print();
  }

  Function::Print() {
    std::cout << "This is a function with name " << name << std::endl;
    std::cout << "and num_arguments " << arguments << std::endl;
    std::cout << "and num_locals " << locals << std::endl;
    std::cout << "and its instructions are: ";
    for (auto step : instructions) {
      step.Print();
    }
  }

  Program::Print() {
    std::cout << "This is a Program with entryPointLabel " << entryPointLabel << std::endl;
    std::cout << "and its functions are: ";
    for (auto fn : functions) {
      fn.Print();
    }
  }

}
