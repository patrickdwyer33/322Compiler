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

#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>

#include <L1.h>
#include <parser.h>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

namespace L1 {

  std::vector<Item *> parsed_items;

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

  struct argument_number:
    Number {};

  struct local_number:
    Number {} ;

  struct comment: 
    pegtl::disable< 
      TAOCPP_PEGTL_STRING( "//" ), 
      pegtl::until< pegtl::eolf > 
    > {};

  struct u:
    pegtl::sor<
      Register,
      Label
    >{};

  struct t:
    pegtl::sor<
      Register,
      Number
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

  struct Location:
    pegtl::sor<
      Register,
      MemoryLocation
    >{};

  struct seps: 
    pegtl::star< 
      pegtl::sor< 
        pegtl::ascii::space, 
        comment 
      > 
    > {};

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
      >
      pegtl::one<'='>
    >{};

  struct Lea:
    pegtl::seq<
      Register,
      seps,
      pegtl::one<'@'>,
      seps,
      Register,
      seps,
      Register,
      seps,
      Number
    >{};

  struct Operation:
    pegtl::sor<
      one_item_op,
      two_item_op,
      Lea
    >{};

  struct cmp:
    pegtl::sor<
      pegtl::seq<
        pegtl::one<'<'>,
        pegtl::one<'='>
      >
      pegtl::one<'<'>,
      pegtl::one<'='>
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
      TAOCPP_PEGTL_STRING("rsp"),
    >{};

  struct MemoryLocation:
    pegtl::seq<
      TAOCPP_PEGTL_STRING("mem"),
      seps,
      Register,
      seps,
      number
    >{};

  struct Instruction_return : TAOCPP_PEGTL_STRING("return") {};

  struct Instruction_assignment:
    pegtl::seq<
      Location,
      seps,
      str_arrow,
      seps,
      s
    >{};

  struct Instruction_operation1:
    pegtl::seq<
      Register,
      seps,
      one_item_op
    >{};

  struct Instruction_operation2:
    pegtl::seq<
      Location,
      seps,
      two_item_op,
      seps,
      Value
    >{};

  struct Instruction_operation3:
    Lea {};

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
      Register,
      seps,
      str_arrow,
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
      Label
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
      Number
    >{};

  struct Instruction_call_input:
    pegtl::seq<
      call_str,
      seps,
      input_str,
      seps,
      Number
    >{};

  struct Instruction_call_allocate:
    pegtl::seq<
      call_str,
      seps,
      allocate_str,
      seps,
      Number
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
      pegtl::seq< pegtl::at<Instruction_return>, Instruction_return>,
      pegtl::seq< pegtl::at<Instruction_save_cmp>, Instruction_save_cmp>,
      pegtl::seq< pegtl::at<Instruction_assignment>, Instruction_assignment>,
      pegtl::seq< pegtl::at<Instruction_operation1>, Instruction_operation1>,
      pegtl::seq< pegtl::at<Instruction_operation2>, Instruction_operation2>,
      pegtl::seq< pegtl::at<Instruction_cjump>, Instruction_cjump>,
      pegtl::seq< pegtl::at<Instruction_save_cmp>, Instruction_save_cmp>
      pegtl::seq< pegtl::at<Instruction_goto>, Instruction_goto>,
      pegtl::seq< pegtl::at<Instruction_call>, Instruction_call>,
      pegtl::seq< pegtl::at<Instruction_call_print>, Instruction_call_print>,
      pegtl::seq< pegtl::at<Instruction_call_input>, Instruction_call_input>,
      pegtl::seq< pegtl::at<Instruction_call_allocate>, Instruction_call_allocate>,
      pegtl::seq< pegtl::at<Instruction_call_tensorError>, Instruction_call_tensorError>,
      pegtl::seq< pegtl::at<Instruction_label>, Instruction_label>
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
    > {};

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

  struct grammar : 
    pegtl::must< 
      entry_point_rule
    > {};

  /* 
   * Actions attached to grammar rules.
   */
  template<typename Rule>
  struct action : pegtl::nothing<Rule> {};

  template<> struct action <function_name_rule> {
    template<typename Input>
	  static void apply(const Input & in, Program & p) {
      if (p.entryPointLabel.empty()){
        p.entryPointLabel = in.string();
      } else {
        L1::Function* newF = new Function();
        newF->name = in.string();
        p.functions.push_back(newF);
      }
    }
  };

  template<> struct action <argument_number> {
    template<typename Input>
	  static void apply(const Input & in, Program & p) {
      L1::Function* currentF = p.functions.back();
      currentF->arguments = std::stoll(in.string());
    }
  };

  template<> struct action <local_number> {
    template<typename Input>
	  static void apply(const Input & in, Program & p) {
      L1::Function* currentF = p.functions.back();
      currentF->locals = std::stoll(in.string());
    }
  };

  template<> struct action <Number> {
    template<typename Input>
	  static void apply(const Input & in, Program & p) {
      L1::Function* currentF = p.functions.back();

      L1::Number* n = new L1::Number(std::stoll(in.string()));

      parsed_items.push_back(n);
    }
  };

  template<> struct action <Label> {
    template<typename Input>
    static void apply(const Input & in, Program & p) {
      L1::Label* l = new L1::Label(in.string());
      parsed_items.push_back(l);
    }
  };

  template<> struct action <Register> {
    template< typename Input >
    static void apply( const Input & in, Program & p) {
      std::string s = in.string();
      Architecture::RegisterID reg_id = Architecture::reg_from_string(s);
      L1::Register* r = new L1::Register(reg_id);
      parsed_items.push_back(r);
    }
  };

  template<> struct action <MemoryLocation> {
    template<typename Input>
    static void apply(const Input & in, Program & p) {
      L1::Number* n = parsed_items.back();
      parsed_items.pop_back();
      L1::Register* r = parsed_items.back();
      parsed_items.pop_back();
      L1::MemoryLocation* mem = new L1::MemoryLocation(r, n);
      parsed_items.push_back(mem);
    }
  };

  template<> struct action <MemoryLocation> {
    template<typename Input>
    static void apply(const Input & in, Program & p) {
      Architecture::OP opID = Architecture::OP_from_string(in.string());
      L1::Operation* op = new L1::Operation(opID);
      parsed_items.push_back(op);
    }
  };

  template<> struct action <Instruction_return> {
    template<typename Input>
	  static void apply(const Input & in, Program & p) {
      L1::Function* currentF = p.functions.back();
      L1::Number* i = new L1::Instruction_ret();
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_assignment> {
    template<typename Input>
	  static void apply(const Input & in, Program & p) {
      L1::Function* currentF = p.functions.back();

      auto src = parsed_items.back();
      parsed_items.pop_back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();

      L1::Instruction_assignment* i = new L1::Instruction_assignment(dst, src);

      currentF->instructions.push_back(i);
    }
  };

  // operation with 1 item
  template<> struct action <Instruction_operation1> {
    template<typename Input>
	  static void apply(const Input & in, Program & p) {
      L1::Function* currentF = p.functions.back();

      L1::Operation* op = parsed_items.back();
      parsed_items.pop_back();
      L1::Register* reg = parsed_items.back();
      parsed_items.pop_back();

      L1::NullItem* emptyThing;

      L1::Instruction_operation* i = new Instruction_operation(reg, op, emptyThing, emptyThing, emptyThing);

      currentF->instructions.push_back(i);
    }
  };

  // operation with 2 items
  template<> struct action <Instruction_operation2> {
    template<typename Input>
	  static void apply(const Input & in, Program & p) {
      L1::Function* currentF = p.functions.back();

      auto left = parsed_items.back();
      parsed_items.pop_back();
      L1::Operation* op = parsed_items.back();
      parsed_items.pop_back();
      auto right = parsed_items.back();
      parsed_items.pop_back();

      L1::Instruction_operation* i = new Instruction_operation(left, op, right, emptyThing, emptyThing);

      currentF->instructions.push_back(i);
    }
  };

  // lea (3-ish items)
  template<> struct action <Instruction_operation3> {
    template<typename Input>
	  static void apply(const Input & in, Program & p) {
      L1::Function* currentF = p.functions.back();

      L1::Number* factor = parsed_items.back();
      parsed_items.pop_back();
      L1::Register* second = parsed_items.back();
      parsed_items.pop_back();
      L1::Register* first = parsed_items.back();
      parsed_items.pop_back();
      L1::Operation* op = parsed_items.back();
      parsed_items.pop_back();
      L1::Register* dst = parsed_items.back();
      parsed_items.pop_back();

      L1::Instruction_operation* i = new Instruction_operation(dst, op, first, second, factor);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_cjump> {
    template<typename Input>
    static void apply(const Input & in, Program & p){
      L1::Function* currentF = p.functions.back();

      L1::Label* label = parsed_items.back();
      parsed_items.pop_back();
      auto right = parsed_items.back();
      parsed_items.pop_back();
      L1::CompareOP* cmpOP = parsed_items.back();
      parsed_items.pop_back();
      auto left = parsed_items.back();
      parsed_items.pop_back();

      L1::Instruction_cjump* i = new Instruction_cjump(left, cmpOP, right, label);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_save_cmp> {
    template<typename Input>
    static void apply(const Input & in, Program & p){
      L1::Function* currentF = p.functions.back();

      auto right = parsed_items.back();
      parsed_items.pop_back();
      L1::CompareOP* cmpOP = parsed_items.back();
      parsed_items.pop_back();
      auto left = parsed_items.back();
      parsed_items.pop_back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();

      L1::Instruction_save_cmp i = new Instruction_save_cmp(dst, left, cmpOP, right);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_label> {
    template<typename Input>
    static void apply(const Input & in, Program & p){
      L1::Function* currentF = p.functions.back();

      L1::Label* label = parsed_items.back();
      parsed_items.pop_back();

      L1::Instruction_label* i = new Instruction_label(label);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_goto> {
    template<typename Input>
    static void apply(const Input & in, Program & p){
      L1::Function* currentF = p.functions.back();

      L1::Label* label = parsed_items.back();
      parsed_items.pop_back();

      L1::Instruction_goto* i = new Instruction_goto(label);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_call> {
    template<typename Input>
	  static void apply(const Input & in, Program & p){
      L1::Function* currentF = p.functions.back();

      L1::Number* num_args = parsed_items.back();
      parsed_items.pop_back();
      auto fn = parsed_items.back();
      parsed_items.pop_back();

      L1::Instruction_call* i = new Instruction_call(fn, num_args)

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_call_print> {
    template<typename Input>
	  static void apply(const Input & in, Program & p){
      L1::Function* currentF = p.functions.back();

      parsed_items.pop_back();

      L1::Instruction_call_print* i = new Instruction_call_print();

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_call_input> {
    template<typename Input>
	  static void apply(const Input & in, Program & p){
      L1::Function* currentF = p.functions.back();

      parsed_items.pop_back();

      L1::Instruction_call_input* i = new Instruction_call_input();

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_call_allocate> {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      L1::Function* currentF = p.functions.back();

      parsed_items.pop_back();

      L1::Instruction_call_allocate* i = new Instruction_call_allocate();

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_call_tensorError> {
    template<typename Input>
	  static void apply(const Input & in, Program & p){
      L1::Function* currentF = p.functions.back();

      L1::Number* arg = parsed_items.back();
      parsed_items.pop_back();

      L1::Instruction_call_tensorError* i = new Instruction_call_tensorError(arg);

      currentF->instructions.push_back(i);
    }
  };

  Program parse_file (char *fileName){

    /* 
     * Check the grammar for some possible issues.
     */
    pegtl::analyze<grammar>();

    /*
     * Parse.
     */
    file_input<> fileInput(fileName);
    Program p;
    parse<grammar, action>(fileInput, p);

    return p;
  }

}
