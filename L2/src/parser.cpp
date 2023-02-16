/*
 * SUGGESTION FROM THE CC TEAM:
 * double check the order of actions that are fired.
 * You can do this in (at least) two ways:
 * 1) by using gdb adding breakpoints to actions
 * 2) by adding printing statements in each action
 *
 * For 2), we suggest writing the code to make it straightforward to enable/disable all of them 
 * (e.g., assuming shouldIPrint is a global variable
 *    if (shouldIPrint) std::cerr << "MY OUTPUT" << std::endl;
 * )
 */
#include <sched.h>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <set>
#include <iterator>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <stdint.h>
#include <assert.h>
#include <iostream>
#include <typeinfo>

#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>

#include <L2.h>
#include <parser.h>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

using namespace pegtl;

namespace Parser {

  std::vector<L2::Item *> parsed_items;

  struct name:
    pegtl::seq<
      pegtl::plus< 
        pegtl::sor<
          pegtl::alpha,
          pegtl::one<'_'>
        >
      >,
      pegtl::star<
        pegtl::sor<
          pegtl::alpha,
          pegtl::one<'_'>,
          pegtl::digit
        >
      >
    > {};

  struct Number:
    pegtl::seq<
      pegtl::opt<
        pegtl::sor<
          pegtl::one<'-'>,
          pegtl::one<'+'>
        >
      >,
      pegtl::plus< 
        pegtl::digit
      >
    >{};

  struct argument_number:
    Number {};

  struct local_number:
    Number {} ;

  struct comment: 
    pegtl::disable< 
      TAOCPP_PEGTL_STRING( "//" ), 
      pegtl::until< pegtl::eolf > 
    > {};

  struct seps: 
    pegtl::star< 
      pegtl::sor< 
        pegtl::ascii::space, 
        comment 
      > 
    > {};

  struct Register:
    pegtl::sor<
      TAOCPP_PEGTL_STRING("rdi"),
      TAOCPP_PEGTL_STRING("rax"),
      TAOCPP_PEGTL_STRING("rsi"),
      TAOCPP_PEGTL_STRING("rdx"),
      TAOCPP_PEGTL_STRING("rcx"),
      TAOCPP_PEGTL_STRING("r8"),
      TAOCPP_PEGTL_STRING("r9"),
      TAOCPP_PEGTL_STRING("rbx"),
      TAOCPP_PEGTL_STRING("rbp"),
      TAOCPP_PEGTL_STRING("r10"),
      TAOCPP_PEGTL_STRING("r11"),
      TAOCPP_PEGTL_STRING("r12"),
      TAOCPP_PEGTL_STRING("r13"),
      TAOCPP_PEGTL_STRING("r14"),
      TAOCPP_PEGTL_STRING("r15"),
      TAOCPP_PEGTL_STRING("rsp")
    >{};

  struct Label:
    pegtl::sor<
      pegtl::seq<
        pegtl::one<':'>,
        name
      >,
      pegtl::seq<
        pegtl::one<'@'>,
        name
      >
    > {};

  struct StackArg:
    pegtl::seq<
      TAOCPP_PEGTL_STRING("stack-arg"),
      seps,
      Number
    >{};

  struct Variable:
    pegtl::seq<
      pegtl::one<'%'>,
      name
    >{};

  struct Var:
    pegtl::sor<
      Variable,
      Register
    >{};

  struct MemoryLocation:
    pegtl::seq<
      TAOCPP_PEGTL_STRING("mem"),
      seps,
      Var,
      seps,
      Number
    >{};

  struct u:
    pegtl::sor<
      Var,
      Label
    >{};

  struct t:
    pegtl::sor<
      Var,
      Number
    >{};

  struct Location:
    pegtl::sor<
      Var,
      MemoryLocation
    >{};

  struct Value:
    pegtl::sor<
      Location,
      Number
    >{};

  struct s:
    pegtl::sor<
      Label,
      t
    >{};

  struct one_item_op:
    pegtl::sor<
      pegtl::seq<
        pegtl::one<'+'>,
        pegtl::one<'+'>
      >,
      pegtl::seq<
        pegtl::one<'-'>,
        pegtl::one<'-'>
      >
    >{};

  struct two_item_op:
    pegtl::seq<
      pegtl::sor<
        pegtl::one<'+'>,
        pegtl::one<'-'>,
        pegtl::one<'*'>,
        pegtl::one<'&'>,
        pegtl::seq<
          pegtl::one<'<'>,
          pegtl::one<'<'>
        >,
        pegtl::seq<
          pegtl::one<'>'>,
          pegtl::one<'>'>
        >
      >,
      pegtl::one<'='>
    >{};

  struct lea:
    pegtl::one<'@'>{};

  struct operation:
    pegtl::sor<
      one_item_op,
      two_item_op,
      lea
    >{};

  struct Instruction_operation3:
    pegtl::seq<
      Var,
      seps,
      pegtl::seq<
        pegtl::at<lea>,
        operation
      >,
      seps,
      Var,
      seps,
      Var,
      seps,
      Number
    >{};

  struct cmp:
    pegtl::sor<
      pegtl::seq<
        pegtl::one<'<'>,
        pegtl::one<'='>
      >,
      pegtl::one<'='>,
      pegtl::one<'<'>
    >{};

  struct str_arrow :
    pegtl::seq<
      pegtl::one<'<'>,
      pegtl::one<'-'>
    >{};

  struct cjump_str: TAOCPP_PEGTL_STRING("cjump") {};
  struct goto_str: TAOCPP_PEGTL_STRING("goto") {};
  struct call_str: TAOCPP_PEGTL_STRING("call") {};
  struct print_str: TAOCPP_PEGTL_STRING("print") {};
  struct input_str: TAOCPP_PEGTL_STRING("input") {};
  struct allocate_str: TAOCPP_PEGTL_STRING("allocate") {};
  struct tensorError_str: TAOCPP_PEGTL_STRING("tensor-error") {};

  struct function_name_rule:
    pegtl::seq<
      pegtl::one<'@'>,
      name
    > {};

  struct Instruction_return : TAOCPP_PEGTL_STRING("return") {};

  struct storable:
    pegtl::sor<
      s,
      MemoryLocation,
      StackArg
    >{};

  struct Instruction_assignment:
    pegtl::seq<
      Location,
      seps,
      str_arrow,
      seps,
      storable
    >{};

  struct Instruction_operation1:
    pegtl::seq<
      Var,
      seps,
      pegtl::seq<
        pegtl::at<one_item_op>,
        operation
      >
    >{};

  struct Instruction_operation2:
    pegtl::seq<
      Location,
      seps,
      pegtl::seq<
        pegtl::at<two_item_op>,
        operation
      >,
      seps,
      Value
    >{};

  struct Instruction_cjump:
    pegtl::seq<
      cjump_str,
      seps,
      t,
      seps,
      cmp,
      seps,
      t,
      seps,
      Label
    >{};

  struct Instruction_save_cmp:
    pegtl::seq<
      Var,
      seps,
      str_arrow,
      seps,
      t,
      seps,
      cmp,
      seps,
      t
    >{};

  struct Instruction_goto:
    pegtl::seq<
      goto_str,
      seps,
      Label
    >{};

  struct Instruction_label:
    pegtl::seq<
      pegtl::one<':'>,
      name
    >{};

  struct Instruction_call:
    pegtl::seq<
      call_str,
      seps,
      u,
      seps,
      Number
    >{};

  struct Instruction_call_print:
    pegtl::seq<
      call_str,
      seps,
      print_str,
      seps,
      pegtl::one<'1'>
    >{};

  struct Instruction_call_input:
    pegtl::seq<
      call_str,
      seps,
      input_str,
      seps,
      pegtl::one<'0'>
    >{};

  struct Instruction_call_allocate:
    pegtl::seq<
      call_str,
      seps,
      allocate_str,
      seps,
      pegtl::one<'2'>
    >{};

  struct Instruction_call_tensorError:
    pegtl::seq<
      call_str,
      seps,
      tensorError_str,
      seps,
      Number
    >{};

  struct Instruction_rule:
    pegtl::sor<
      pegtl::seq< pegtl::at<Instruction_save_cmp>, Instruction_save_cmp>,
      pegtl::seq< pegtl::at<Instruction_assignment>, Instruction_assignment>,
      pegtl::seq< pegtl::at<Instruction_operation3>, Instruction_operation3>,
      pegtl::seq< pegtl::at<Instruction_operation2>, Instruction_operation2>,
      pegtl::seq< pegtl::at<Instruction_cjump>, Instruction_cjump>,
      pegtl::seq< pegtl::at<Instruction_goto>, Instruction_goto>,
      pegtl::seq< pegtl::at<Instruction_call>, Instruction_call>,
      pegtl::seq< pegtl::at<Instruction_call_print>, Instruction_call_print>,
      pegtl::seq< pegtl::at<Instruction_call_input>, Instruction_call_input>,
      pegtl::seq< pegtl::at<Instruction_call_allocate>, Instruction_call_allocate>,
      pegtl::seq< pegtl::at<Instruction_call_tensorError>, Instruction_call_tensorError>,
      pegtl::seq< pegtl::at<Instruction_operation1>, Instruction_operation1>,
      pegtl::seq< pegtl::at<Instruction_label>, Instruction_label>,
      pegtl::seq< pegtl::at<Instruction_return>, Instruction_return>
    > { };

  struct Instructions_rule:
    pegtl::plus<
      pegtl::seq<
        seps,
        Instruction_rule,
        seps
      >
    > { };

  struct Function_rule:
    pegtl::seq<
      seps,
      pegtl::one< '(' >,
      seps,
      function_name_rule,
      seps,
      argument_number,
      seps,
      local_number,
      seps,
      Instructions_rule,
      seps,
      pegtl::one< ')' >
    >{};

  struct Functions_rule:
    pegtl::plus<
      seps,
      Function_rule,
      seps
    > {};

  struct entry_point_rule:
    pegtl::seq<
      seps,
      pegtl::one< '(' >,
      seps,
      function_name_rule,
      seps,
      Functions_rule,
      seps,
      pegtl::one< ')' >,
      seps
    > { };

  struct single_function_name_rule:
    pegtl::seq<
      pegtl::one<'@'>,
      name
    > {};

  struct single_function_entry_point_rule:
    pegtl::seq<
      seps,
      pegtl::one< '(' >,
      seps,
      single_function_name_rule,
      seps,
      argument_number,
      seps,
      Instructions_rule,
      seps,
      pegtl::one< ')' >
    >{};

  struct spill_entry_point_rule:
    pegtl::seq<
      single_function_entry_point_rule,
      seps,
      Variable,
      seps,
      Variable
    > { };

  struct grammar : 
    pegtl::must< 
      entry_point_rule
    > {};

  struct spill_grammer : 
    pegtl::must<
      spill_entry_point_rule
    > {};

  struct single_function_grammer : 
    pegtl::must<
      single_function_entry_point_rule
    > {};

  /* 
   * Actions attached to grammar rules.
   */
  template<typename Rule>
  struct action : pegtl::nothing<Rule> {};

  template<> struct action <single_function_name_rule> {
    template<typename Input>
	  static void apply(const Input & in, L2::Program & p) {
      L2::Function* newF = new L2::Function();
      newF->name = in.string();
      p.functions.push_back(newF);
    }
  };

  //change
  template<> struct action <spill_entry_point_rule> {
    template<typename Input>
	  static void apply(const Input & in, L2::Program & p) {
      auto prefix = parsed_items.back();
      L2::Variable* prefix_var = (L2::Variable*)prefix;
      parsed_items.pop_back();
      auto to_spill = parsed_items.back();
      L2::Variable* to_spill_var = (L2::Variable*)to_spill;
      parsed_items.pop_back();
      p.prefix_var = prefix_var;
      p.to_spill_var = to_spill_var;
    }
  };

  template<> struct action <function_name_rule> {
    template<typename Input>
	  static void apply(const Input & in, L2::Program & p) {
      if (p.entryPointLabel.empty()){
        p.entryPointLabel = in.string();
      } else {
        L2::Function* newF = new L2::Function();
        newF->name = in.string();
        p.functions.push_back(newF);
      }
    }
  };

  template<> struct action <argument_number> {
    template<typename Input>
	  static void apply(const Input & in, L2::Program & p) {
      L2::Function* currentF = p.functions.back();
      currentF->arguments = std::stoll(in.string());
    }
  };

  template<> struct action <Number> {
    template<typename Input>
	  static void apply(const Input & in, L2::Program & p) {
      L2::Function* currentF = p.functions.back();

      L2::Number* n = new L2::Number(std::stoll(in.string()));

      parsed_items.push_back(n);
    }
  };

  template<> struct action <Label> {
    template<typename Input>
    static void apply(const Input & in, L2::Program & p) {
      L2::Label* l = new L2::Label(in.string());
      parsed_items.push_back(l);
    }
  };

  template<> struct action <Register> {
    template<typename Input>
    static void apply( const Input & in, L2::Program & p) {
      std::string s = in.string();
      Architecture::RegisterID reg_id = Architecture::reg_from_string(s);
      L2::Register* r = new L2::Register(s, reg_id);
      parsed_items.push_back(r);
    }
  };

  template<> struct action <Variable> {
    template<typename Input>
    static void apply( const Input & in, L2::Program & p) {
      L2::Variable* v = new L2::Variable(in.string());
      parsed_items.push_back(v);
    }
  };

  template<> struct action <MemoryLocation> {
    template<typename Input>
    static void apply(const Input & in, L2::Program & p) {
      auto n = parsed_items.back();
      L2::Number* real_n = (L2::Number*)n;
      parsed_items.pop_back();
      auto v = parsed_items.back();
      L2::Variable* real_v = (L2::Variable*)v;
      parsed_items.pop_back();
      L2::MemoryLocation* mem = new L2::MemoryLocation(real_v, real_n);
      parsed_items.push_back(mem);
    }
  };

  template<> struct action <StackArg> {
    template<typename Input>
    static void apply(const Input & in, L2::Program & p) {
      auto n = parsed_items.back();
      L2::Number* real_n = (L2::Number*)n;
      parsed_items.pop_back();
      L2::StackArg* arg = new L2::StackArg(real_n);
      parsed_items.push_back(arg);
    }
  };

  template<> struct action <operation> {
    template<typename Input>
    static void apply(const Input & in, L2::Program & p) {
      Architecture::OP opID = Architecture::OP_from_string(in.string());
      L2::Operation* op = new L2::Operation(opID);
      parsed_items.push_back(op);
    }
  };

  template<> struct action <cmp> {
    template<typename Input>
    static void apply(const Input & in, L2::Program & p) {
      Architecture::CompareOP cmpOPID = Architecture::cmpOP_from_string(in.string());
      L2::CmpOperation* cmpOP = new L2::CmpOperation(cmpOPID);
      parsed_items.push_back(cmpOP);
    }
  };

  template<> struct action <Instruction_return> {
    template<typename Input>
	  static void apply(const Input & in, L2::Program & p) {
      L2::Function* currentF = p.functions.back();
      int64_t num_locals = currentF->locals;
      int64_t num_args = currentF->arguments;
      L2::Number* num_locals_item = new L2::Number(num_locals);
      L2::Number* num_args_item = new L2::Number(num_args);
      L2::Instruction_return* i = new L2::Instruction_return(num_locals_item, num_args_item);
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_assignment> {
    template<typename Input>
	  static void apply(const Input & in, L2::Program & p) {
      L2::Function* currentF = p.functions.back();

      auto src = parsed_items.back();
      parsed_items.pop_back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();

      L2::Instruction_assignment* i = new L2::Instruction_assignment(dst, src);

      currentF->instructions.push_back(i);
    }
  };

  // operation with 1 item
  template<> struct action <Instruction_operation1> {
    template<typename Input>
	  static void apply(const Input & in, L2::Program & p) {
      L2::Function* currentF = p.functions.back();

      auto op = parsed_items.back();
      parsed_items.pop_back();
      auto reg = parsed_items.back();
      parsed_items.pop_back();

      L2::NullItem* emptyThing = new L2::NullItem();
      L2::NullItem* emptyThing2 = new L2::NullItem();
      L2::NullItem* emptyThing3 = new L2::NullItem();
      auto test_name = typeid(*emptyThing).name();
      std::cout << test_name << std::endl;

      L2::Instruction_operation* i = new L2::Instruction_operation(reg, op, emptyThing, emptyThing2, emptyThing3);

      currentF->instructions.push_back(i);
    }
  };

  // operation with 2 items
  template<> struct action <Instruction_operation2> {
    template<typename Input>
	  static void apply(const Input & in, L2::Program & p) {
      L2::Function* currentF = p.functions.back();

      auto right = parsed_items.back();
      parsed_items.pop_back();
      auto op = parsed_items.back();
      parsed_items.pop_back();
      auto left = parsed_items.back();
      parsed_items.pop_back();

      L2::NullItem* emptyThing = new L2::NullItem();
      L2::NullItem* emptyThing2 = new L2::NullItem();

      L2::Instruction_operation* i = new L2::Instruction_operation(left, op, right, emptyThing, emptyThing2);

      currentF->instructions.push_back(i);
    }
  };

  // lea (3-ish items)
  template<> struct action <Instruction_operation3> {
    template<typename Input>
	  static void apply(const Input & in, L2::Program & p) {
      L2::Function* currentF = p.functions.back();

      auto factor = parsed_items.back();
      parsed_items.pop_back();
      auto second = parsed_items.back();
      parsed_items.pop_back();
      auto first = parsed_items.back();
      parsed_items.pop_back();
      auto op = parsed_items.back();
      parsed_items.pop_back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();

      L2::Instruction_operation* i = new L2::Instruction_operation(dst, op, first, second, factor);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_cjump> {
    template<typename Input>
    static void apply(const Input & in, L2::Program & p){
      L2::Function* currentF = p.functions.back();

      auto label = parsed_items.back();
      parsed_items.pop_back();
      auto right = parsed_items.back();
      parsed_items.pop_back();
      auto cmpOP = parsed_items.back();
      parsed_items.pop_back();
      auto left = parsed_items.back();
      parsed_items.pop_back();

      L2::Instruction_cjump* i = new L2::Instruction_cjump(left, cmpOP, right, label);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_save_cmp> {
    template<typename Input>
    static void apply(const Input & in, L2::Program & p){
      L2::Function* currentF = p.functions.back();

      auto right = parsed_items.back();
      parsed_items.pop_back();
      auto cmpOP = parsed_items.back();
      parsed_items.pop_back();
      auto left = parsed_items.back();
      parsed_items.pop_back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();

      L2::Instruction_save_cmp* i = new L2::Instruction_save_cmp(dst, left, cmpOP, right);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_label> {
    template<typename Input>
    static void apply(const Input & in, L2::Program & p){
      L2::Function* currentF = p.functions.back();
      auto idx = currentF->instructions.size();
      currentF->label_map[in.string()] = idx;
      L2::Label* label = new L2::Label(in.string());

      L2::Instruction_label* i = new L2::Instruction_label(label);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_goto> {
    template<typename Input>
    static void apply(const Input & in, L2::Program & p){
      L2::Function* currentF = p.functions.back();

      auto label = parsed_items.back();
      parsed_items.pop_back();

      L2::Instruction_goto* i = new L2::Instruction_goto(label);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_call> {
    template<typename Input>
	  static void apply(const Input & in, L2::Program & p){
      L2::Function* currentF = p.functions.back();

      auto num_args = parsed_items.back();
      parsed_items.pop_back();
      auto fn = parsed_items.back();
      parsed_items.pop_back();

      L2::Instruction_call* i = new L2::Instruction_call(fn, num_args);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_call_print> {
    template<typename Input>
	  static void apply(const Input & in, L2::Program & p){
      L2::Function* currentF = p.functions.back();

      L2::Instruction_call_print* i = new L2::Instruction_call_print();

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_call_input> {
    template<typename Input>
	  static void apply(const Input & in, L2::Program & p){
      L2::Function* currentF = p.functions.back();

      L2::Instruction_call_input* i = new L2::Instruction_call_input();

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_call_allocate> {
    template< typename Input >
	  static void apply( const Input & in, L2::Program & p){
      L2::Function* currentF = p.functions.back();

      L2::Instruction_call_allocate* i = new L2::Instruction_call_allocate();

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_call_tensorError> {
    template<typename Input>
	  static void apply(const Input & in, L2::Program & p){
      L2::Function* currentF = p.functions.back();

      auto arg = parsed_items.back();
      parsed_items.pop_back();

      L2::Instruction_call_tensorError* i = new L2::Instruction_call_tensorError(arg);

      currentF->instructions.push_back(i);
    }
  };

  L2::Program parse_file (char *fileName){

    /* 
     * Check the grammar for some possible issues.
     */
    pegtl::analyze<grammar>();

    /*
     * Parse.
     */
    pegtl::file_input< > fileInput(fileName);
    L2::Program p;
    pegtl::parse<grammar, action>(fileInput, p);

    return p;
  }

  L2::Program parse_spill_file (char *fileName){

    /* 
     * Check the grammar for some possible issues.
     */
    pegtl::analyze<spill_grammer>();

    /*
     * Parse.
     */
    pegtl::file_input< > fileInput(fileName);
    L2::Program p;
    pegtl::parse<spill_grammer, action>(fileInput, p);

    return p;
  }

  L2::Program parse_function_file (char *fileName){

    /* 
     * Check the grammar for some possible issues.
     */
    pegtl::analyze<single_function_grammer>();

    /*
     * Parse.
     */
    pegtl::file_input< > fileInput(fileName);
    L2::Program p;
    pegtl::parse<single_function_grammer, action>(fileInput, p);

    return p;
  }

}
