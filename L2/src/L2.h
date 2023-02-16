#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <Architecture.h>


namespace L2 {

  class Item {
    public:
      virtual std::string to_string() = 0;
  };

  class NullItem : public Item {
    public:
      NullItem();
      std::string to_string() override;
  };

  class Number : public Item {
    public:
      Number(int64_t value);
      std::string to_string() override;
      int64_t get() const;
    private:
      int64_t value;
  };

  class Label : public Item {
    public:
      Label(std::string value);
      std::string to_string() override;
      std::string get() const;
    private:
      std::string value;
  };

  class Variable : public Item {
    public:
      Variable(std::string s);
      std::string to_string() override;
      std::string get() const;
    protected:
      std::string s;
  };

  class Register : public Variable {
    public:
      Register(std::string s, Architecture::RegisterID r) : Variable(s) {
        this->ID = r;
      };
      Architecture::RegisterID get_reg() const;
    private:
      Architecture::RegisterID ID;
  };

  class StackArg : public Item {
    public:
      StackArg(Number* offset);
      std::string to_string() override;
      Number* get() const;
    private:
      Number* offset;
  };

  class MemoryLocation : public Item {
    public:
      MemoryLocation(Variable* base, Number* offset);
      std::string to_string() override;
      std::vector<Item*> get() const;
    private:
      Variable* base_var;
      Number* offset;
  };

  class Operation : public Item {
    public:
      Operation(Architecture::OP);
      std::string to_string() override;
      Architecture::OP get() const;
    private:
      Architecture::OP OP;
  };

  class CmpOperation : public Item {
    public:
      CmpOperation(Architecture::CompareOP);
      std::string to_string() override;
      Architecture::CompareOP get() const;
    private:
      Architecture::CompareOP cmpOP;
  };

  /*
   * Instruction interface.
   */

  class Visitor;

  class Instruction {
    public:
      virtual std::string to_string() = 0;
      virtual void accept(Visitor* v) = 0;
      std::unordered_set<L2::Variable*> gen;
      std::unordered_set<L2::Variable*> kill;
      int idx;
      std::unordered_set<int> succs;
  };

  /*
   * Instructions.
   */
  class Instruction_return : public Instruction {
    public:
      Instruction_return(Item* num_locals, Item* num_args);
      std::string to_string() override;
      void accept(Visitor* v);
      std::vector<Item*> get() const;
    private:
      Item* num_locals;
      Item* num_args;
  };

  class Instruction_assignment : public Instruction {
    public:
      Instruction_assignment(Item* dst, Item* src);
      std::string to_string() override;
      void accept(Visitor* v);
      std::vector<Item*> get() const;
    private:
      Item* dst;
      Item* src;
  };

  class Instruction_operation : public Instruction{
    public:
      Instruction_operation(Item* left, Item* op, Item* right, Item* lea_reg, Item* factor);
      std::string to_string() override;
      void accept(Visitor* v);
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
      std::string to_string() override;
      void accept(Visitor* v);
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
      std::string to_string() override;
      void accept(Visitor* v);
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
      std::string to_string() override;
      void accept(Visitor* v);
      Item* get() const;
    private:
      Item* label;
  };

  class Instruction_goto : public Instruction{
    public:
      Instruction_goto(Item* label);
      std::string to_string() override;
      void accept(Visitor* v);
      Item* get() const;
    private:
      Item* label;
  };

  class Instruction_call : public Instruction {
    public:
      Instruction_call(Item* fn, Item* arg);
      std::string to_string() override;
      void accept(Visitor* v);
      std::vector<Item*> get() const;
    private:
      Item* fn;
      Item* arg;
  };

  class Instruction_call_print : public Instruction {
    public:
      Instruction_call_print();
      std::string to_string() override;
      void accept(Visitor* v);
  };

  class Instruction_call_input : public Instruction {
    public:
      Instruction_call_input();
      std::string to_string() override;
      void accept(Visitor* v);
  };

  class Instruction_call_allocate : public Instruction {
    public:
      Instruction_call_allocate();
      std::string to_string() override;
      void accept(Visitor* v);
  };

  class Instruction_call_tensorError : public Instruction {
    public:
      Instruction_call_tensorError(Item* arg);
      std::string to_string() override;
      void accept(Visitor* v);
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
      std::vector<std::unordered_set<Variable *>> ins;
      std::vector<std::unordered_set<Variable *>> outs;
      // can get this info in the parser by updating it whenever we find a label according to the number of instructions in currentF
      std::unordered_map<std::string, int> label_map;
      void accept(Visitor* v);
      void to_string();
  };

  class Program {
    public:
      std::string entryPointLabel;
      L2::Variable* prefix_var;
      L2::Variable* to_spill_var;
      std::vector<Function *> functions;
      void accept(Visitor* v);
      void to_string();
  };

  class Visitor {
    public:
      virtual void visit(Instruction_return* i) = 0;
      virtual void visit(Instruction_assignment* i) = 0;
      virtual void visit(Instruction_operation* i) = 0;
      virtual void visit(Instruction_cjump* i) = 0;
      virtual void visit(Instruction_save_cmp* i) = 0;
      virtual void visit(Instruction_label* i) = 0;
      virtual void visit(Instruction_goto* i) = 0;
      virtual void visit(Instruction_call* i) = 0;
      virtual void visit(Instruction_call_print* i) = 0;
      virtual void visit(Instruction_call_input* i) = 0;
      virtual void visit(Instruction_call_allocate* i) = 0;
      virtual void visit(Instruction_call_tensorError* i) = 0;
      virtual void visit(Function* fn) = 0;
      virtual void visit(Program* p) = 0;
  };

}
