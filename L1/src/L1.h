#pragma once

#include <vector>
#include <string>
#include <Architecture.h>


namespace L1 {

  class Item {
    public:
      virtual std::string to_string() const = 0;
  };

  class NullItem : public Item {
    public:
      NullItem();
      std::string to_string() const override;
  };

  class Number : public Item {
    public:
      Number(int64_t value);
      std::string to_string() const override;
      int64_t get() const;
      bool operator == (const Number &other) const;
    private:
      int64_t value;
  };

  class Label : public Item {
    public:
      Label(std::string value);
      std::string to_string() const override;
      std::string get() const;
      bool operator == (const Label &other) const;
    private:
      std::string value;
  };

  class Register : public Item {
    public:
      Register(Architecture::RegisterID r);
      std::string to_string() const override;
      Architecture::RegisterID get() const;
      bool operator == (const Register &other) const;
    private:
      Architecture::RegisterID ID;
  };

  class MemoryLocation : public Item {
    public:
      MemoryLocation(Register* r, Number* n);
      std::string to_string() const override;
      std::pair<Register*, Number*> get() const;
      bool operator == (const MemoryLocation &other) const;
    private:
      Register* base_register;
      Number* offset;
  };

  class Operation : public Item {
    public:
      Operation(Architecture::OP);
      std::string to_string() const override;
      Architecture::OP get() const;
      bool operator == (const Operation &other) const;
    private:
      Architecture::OP OP;
  };

  class CmpOperation : public Item {
    public:
      CmpOperation(Architecture::CompareOP);
      std::string to_string() const override;
      Architecture::CompareOP get() const;
      bool operator == (const CmpOperation &other) const;
    private:
      Architecture::CompareOP cmpOP;
  };

  /*
   * Instruction interface.
   */

  class Visitor;

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
      Instruction_return(Item* num_locals);
      std::string to_string() const override;
      void accept(Visitor* v) const;
      Item* get() const;
    private:
      Item* num_locals;
  };

  class Instruction_assignment : public Instruction {
    public:
      Instruction_assignment(Item* dst, Item* src);
      std::string to_string() const override;
      void accept(Visitor* v) const;
      std::vector<Item*> get() const;
    private:
      Item* dst;
      Item* src;
  };

  class Instruction_operation : public Instruction{
    public:
      Instruction_operation(Item* left, Item* op, Item* right, Item* lea_reg, Item* factor);
      std::string to_string() const override;
      void accept(Visitor* v) const;
      std::vector<Item*> get() const;
    private:
      Item* left;
      Item* OP;
      Item* right;
      Item* lea_reg;
      Item* factor;
  };

  class Instruction_cjump : public Instruction{
    public:
      Instruction_cjump(Item* left, Item* cmpOP, Item* right, Item* label);
      std::string to_string() const override;
      void accept(Visitor* v) const;
      std::vector<Item*> get() const;
    private:
      Item* left;
      Item* OP;
      Item* right;
      Item* label;
  };

  class Instruction_save_cmp : public Instruction{
    public:
      Instruction_save_cmp(Item* dst, Item* left, Item* cmpOP, Item* right);
      std::string to_string() const override;
      void accept(Visitor* v) const;
      std::vector<Item*> get() const;
    private:
      Item* dst;
      Item* left;
      Item* OP;
      Item* right;
  };

  class Instruction_label : public Instruction{
    public:
      Instruction_label(Item* label);
      std::string to_string() const override;
      void accept(Visitor* v) const;
      Item* get() const;
    private:
      Item* label;
  };

  class Instruction_goto : public Instruction{
    public:
      Instruction_goto(Item* label);
      std::string to_string() const override;
      void accept(Visitor* v) const;
      Item* get() const;
    private:
      Item* label;
  };

  class Instruction_call : public Instruction {
    public:
      Instruction_call(Item* fn, Item* arg);
      std::string to_string() const override;
      void accept(Visitor* v) const;
      std::vector<Item*> get() const;
    private:
      Item* fn;
      Item* arg;
  };

  class Instruction_call_print : public Instruction {
    public:
      Instruction_call_print();
      std::string to_string() const override;
      void accept(Visitor* v) const;
  };

  class Instruction_call_input : public Instruction {
    public:
      Instruction_call_input();
      std::string to_string() const override;
      void accept(Visitor* v) const;
  };

  class Instruction_call_allocate : public Instruction {
    public:
      Instruction_call_allocate();
      std::string to_string() const override;
      void accept(Visitor* v) const;
  };

  class Instruction_call_tensorError : public Instruction {
    public:
      Instruction_call_tensorError(Item* arg);
      std::string to_string() const override;
      void accept(Visitor* v) const;
      Item* get() const;
    private:
      Item* arg;
  };

  class Function {
    public:
      std::string name;
      int64_t arguments;
      int64_t locals;
      std::vector<Instruction *> instructions;
      void accept(Visitor* v) const;
      void to_string() const;
  };

  class Program {
    public:
      std::string entryPointLabel;
      std::vector<Function *> functions;
      void accept(Visitor* v) const;
      void to_string() const;
  };

  class Visitor {
    public:
      virtual void visit(const Instruction_return* i) = 0;
      virtual void visit(const Instruction_assignment* i) = 0;
      virtual void visit(const Instruction_operation* i) = 0;
      virtual void visit(const Instruction_cjump* i) = 0;
      virtual void visit(const Instruction_save_cmp* i) = 0;
      virtual void visit(const Instruction_label* i) = 0;
      virtual void visit(const Instruction_goto* i) = 0;
      virtual void visit(const Instruction_call* i) = 0;
      virtual void visit(const Instruction_call_print* i) const = 0;
      virtual void visit(const Instruction_call_input* i) const = 0;
      virtual void visit(const Instruction_call_allocate* i) const = 0;
      virtual void visit(const Instruction_call_tensorError* i) const = 0;
      virtual void visit(const Function* fn) = 0;
      virtual void visit(const Program* p) = 0;
  };

}
