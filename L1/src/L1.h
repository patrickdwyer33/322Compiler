#pragma once

#include <vector>
#include <string>

namespace L1 {

  class Item {
    public:
      virtual std::string to_string() = 0;
  };

  class Number : public Item {
    public:
      Number(int64_t value);
      std::string to_string() override const;
      int64_t get() const;
      boolean operator == (const Number &other) const;
    private:
      int64_t value;
  }

  class Label : public Item {
    public:
      Label(std::string value);
      std::string to_string() override const;
      std::string get() const;
      boolean operator == (const Label &other) const;
    private:
      std::string value;
  }

  class Register : public Item {
    public:
      Register(Architecture::RegisterID r);
      std::string to_string() override const;
      Architecture::RegisterID get() const;
      boolean operator == (const Label &other) const;
    private:
      Architecture::RegisterID ID;
  };

  class MemoryLocation : public Item {
    public:
      MemoryLocation(Register* r, Number* n);
      std::string to_string() override const;
      std::pair<Register*, Number*> get() const;
      boolean operator == (const Label &other) const;
    private:
      Register* base_register;
      Number* offset;
  };

  /*
   * Instruction interface.
   */
  class Instruction {
    public:
      virtual std::string to_string() const = 0;
      virtual void accept(Visitor* v) const = 0;
  };

  /*
   * Instructions.
   */
  class Instruction_return : public Instruction {
    public:
      std::string to_string() override const;
      void accept(Visitor* v) const;
  };

  class Instruction_assignment : public Instruction {
    public:
      Instruction_assignment(Item* dst, Item* src);
      std::string to_string() override const;
      void accept(Visitor* v) const;
    private:
      Item* Src;
      Item* Dst;
  };

  class Instruction_operation : public Instruction{
    public:
      Instruction_operation(Architecture::OP op, Item* left, Item* right);
      std::string to_string() override const;
      void accept(Visitor* v) const;
    private:
      Architecture::OP OP;
      Item* left;
      Item* right;
  }

  class Instruction_cjump : public Instruction{
    public:
      Instruction_cjump(Item* left, Architecture::CompareOP, Item* right, Item* label);
      std::string to_string() override const;
      void accept(Visitor* v) const;
    private:
      Architecture::CompareOP OP;
      Item* left;
      Item* right;
      Item* label;
  }

  class Instruction_save_cmp : public Instruction{
    public:
      Instruction_cjump(Item* dst, Item* left, Architecture::CompareOP, Item* right);
      std::string to_string() override const;
      void accept(Visitor* v) const;
    private:
      Item* dst;
      Architecture::CompareOP OP;
      Item* left;
      Item* right;
  }

  class Instruction_goto : public Instruction{
    public:
      Instruction_goto(Label* label);
      std::string to_string() override const;
      void accept(Visitor* v) const;
    private:
      Label* Label;
  }

  class Instruction_call : public Instruction {
    public:
      Instruction_call(Label* name, Number* arg);
      std::string to_string() override const;
      void accept(Visitor* v) const;
    private:
      Label* name;
      Number* Arg;
  }

  class Instruction_call_print : public Instruction {
    public:
      Instruction_call_print();
      std::string to_string() override const;
      void accept(Visitor* v) const;
  }

  class Instruction_call_input : public Instruction {
    public:
      Instruction_call_input();
      std::string to_string() override const;
      void accept(Visitor* v) const;
  }

  class Instruction_call_allocate : public Instruction {
    public:
      Instruction_call_allocate();
      std::string to_string() override const;
      void accept(Visitor* v) const;
  }

  class Instruction_call_tensorError : public Instruction {
    public:
      Instruction_call_tensorError(Number* arg);
      std::string to_string() override const;
      void accept(Visitor* v) const;
    private:
      Number* arg;
  }

  class Function {
    public:
      std::string name;
      int64_t arguments;
      int64_t locals;
      std::vector<Instruction *> instructions;
      void accept(Visitor* v) const;
  };

  class Program {
    public:
      std::string entryPointLabel;
      std::vector<Function *> functions;
      void accept(Visitor* v) const;
  };

  class Visitor {
    public:
      virtual void visit(const Instruction_return* i) = 0;
      virtual void visit(const Instruction_assignment* i) = 0;
      virtual void visit(const Instruction_operation* i) = 0;
      virtual void visit(const Instruction_cjump* i) = 0;
      virtual void visit(const Instruction_goto* i) = 0;
      virtual void visit(const Instruction_call* i) = 0;
      virtual void visit(const Instruction_call_print* i) const = 0;
      virtual void visit(const Instruction_call_input* i) const = 0;
      virtual void visit(const Instruction_call_allocate* i) const = 0;
      virtual void visit(const Instruction_call_tensorError* i) const = 0;
      virtual void visit(const Function* fn) = 0;
      virtual void visit(const Program* p) = 0;
  }

}
