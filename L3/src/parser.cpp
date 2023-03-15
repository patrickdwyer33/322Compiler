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

#include <L3.h>
#include <parser.h>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

using namespace pegtl;

namespace Parser {

  std::vector<L3::Item *> parsed_items;
  
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

  struct var:
    pegtl::seq<
      pegtl::one<'%'>,
      name
    > {};

  struct label:
    pegtl::seq<
      pegtl::one<':'>,
      name
    > {};

  struct fn_name:
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

  struct cmp:
    pegtl::sor<
      pegtl::seq<
        pegtl::one<'<'>,
        pegtl::one<'='>
      >,
      pegtl::seq<
        pegtl::one<'>'>,
        pegtl::one<'='>
      >,
      pegtl::one<'='>,
      pegtl::one<'<'>,
      pegtl::one<'>'>
    > {};

  struct op:
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
    > {};

  struct u:
    pegtl::sor<
      var,
      fn_name
    >{};

  struct t:
    pegtl::sor<
      var,
      Number
    >{};

  struct s:
    pegtl::sor<
      label,
      fn_name,
      t
    >{};

  struct no_args:
    pegtl::seq<
      pegtl::one<'('>,
      seps,
      pegtl::one<')'>
    > {};

  struct one_arg:
    pegtl::seq<
      pegtl::one<'('>,
      seps,
      t,
      seps,
      pegtl::one<')'>
    > {};

  struct many_args:
    pegtl::seq<
      pegtl::one<'('>,
      seps,
      t,
      pegtl::star<
        pegtl::seq<
          seps,
          pegtl::one<','>,
          seps,
          t
        >
      >,
      seps,
      pegtl::one<')'>
    > {};

  struct args:
    pegtl::sor<
      many_args,
      one_arg,
      no_args
    > {};

  struct no_vars:
    pegtl::seq<
      pegtl::one<'('>,
      seps,
      pegtl::one<')'>
    > {};

  struct one_var:
    pegtl::seq<
      pegtl::one<'('>,
      seps,
      var,
      seps,
      pegtl::one<')'>
    > {};

  struct many_vars:
    pegtl::seq<
      pegtl::one<'('>,
      seps,
      var,
      pegtl::star<
        pegtl::seq<
          seps,
          pegtl::one<','>,
          seps,
          var
        >
      >,
      seps,
      pegtl::one<')'>
    > {};

  struct vars:
    pegtl::sor<
      many_vars,
      one_var,
      no_vars
    > {};

  struct callee:
    pegtl::sor<
      u,
      TAOCPP_PEGTL_STRING("print"),
      TAOCPP_PEGTL_STRING("allocate"),
      TAOCPP_PEGTL_STRING("input"),
      TAOCPP_PEGTL_STRING("tensor-error"),
    > {};

  struct Instruction_label:
    label {};

  struct Instruction_return : 
    pegtl::sor<
      pegtl::seq<
        TAOCPP_PEGTL_STRING("return"),
        seps,
        t
      >,
      TAOCPP_PEGTL_STRING("return")
    > {};

  struct Instruction_branch:
    pegtl::sor<
      pegtl::seq<
        TAOCPP_PEGTL_STRING("br"),
        seps,
        t,
        seps,
        label
      >,
      pegtl::seq<
        TAOCPP_PEGTL_STRING("br"),
        seps,
        label
      >
    > {};

  struct Instruction_call:
    pegtl::seq<
      TAOCPP_PEGTL_STRING("call"),
      seps,
      callee,
      seps,
      args
    > {};

  struct Instruction_call_item:
    pegtl::seq<
      TAOCPP_PEGTL_STRING("call"),
      seps,
      callee,
      seps,
      args
    > {};

  struct left_hand:
    pegtl::sor<
      pegtl::seq<
        TAOCPP_PEGTL_STRING("store"),
        seps,
        var,
      >,
      var
    > {};

  struct right_hand:
    pegtl::sor<
      Instruction_call_item,
      pegtl::seq<
        t,
        seps,
        op,
        seps,
        t
      >,
      pegtl::seq<
        t,
        seps,
        cmp,
        seps,
        t
      >,
      pegtl::seq<
        TAOCPP_PEGTL_STRING("load"),
        seps,
        var
      >,
      var
    > {};

  struct str_arrow :
    pegtl::seq<
      pegtl::one<'<'>,
      pegtl::one<'-'>
    >{};


  struct Instruction_assignment:
    pegtl::seq<
      left_hand,
      seps,
      str_arrow,
      seps,
      right_hand
    >{};

  struct Instruction_rule:
    pegtl::sor<
      pegtl::seq< pegtl::at<Instruction_assignment>, Instruction_assignment>,
      pegtl::seq< pegtl::at<Instruction_call>, Instruction_call>,
      pegtl::seq< pegtl::at<Instruction_branch>, Instruction_branch>,
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

  struct fn_declaration:
    pegtl::seq<
      fn_name,
      seps,
      vars
    > {};

  struct Function_rule:
    pegtl::seq<
      seps,
      TAOCPP_PEGTL_STRING("define"),
      seps,
      fn_declaration,
      seps,
      pegtl::one<'{'>,
      seps,
      Instructions_rule,
      seps,
      pegtl::one<'}'>
    >{};

  struct Functions_rule:
    pegtl::plus<
      seps,
      Function_rule,
      seps
    > {};

  struct entry_point_rule:
    pegtl::seq<
      Functions_rule
    > { };

  struct grammar : 
    pegtl::must< 
      entry_point_rule
    > {};


// TODO: UPDATE ACTIONS BELOW


  /* 
   * Actions attached to grammar rules.
   */
  template<typename Rule>
  struct action : pegtl::nothing<Rule> {};

  template<> struct action <fn_declaration> {
    template<typename Input>
	  static void apply(const Input & in, L1::Program & p) {
      L1::Function* newF = new L1::Function();
      // store num args last when vars fires
      Item* num_args_item = parsed_items.back();
      parsed_items.pop_back();
      Number* num_args_num_item = static_cast<Number*>(num_args_item);
      newF->num_args = num_args_num_item->val;
      // make sure the order vals are pushed here is reversed either here or in
      // the vars action
      for (uint64_t i = 0; i < num_args_num_item->val; i++) {
        Item* var = parsed_items.back();
        parsed_items.pop_back();
        newF->vars.push_back(var);
      }
      Item* fn_name = parsed_items.back();
      parsed_items.pop_back();
      newF->name = fn_name->s;
      p.functions.push_back(newF);
    }
  };

  // action for vars
  // make sure to reverse order, and push back the number of vars

  // actions for instruction_call and instruction_call_item
  // don't construct instruction_call in instruction_call_item, just push back
  // the relevant information so instruction_assignment can use it
  // for instruction_assignment, will need to push back contextual information
  // need to push back a context item that we can switch on
  // or, need to seperate instruction_assignment into multiple structs and 
  // respective actions, not sure which way is easier.
  // start out with smaller things so we can know which one is easier before
  // we decide
  // FOLLOW THE ORDER OF THE STRUCTS

  template<> struct action <Number> {
    template<typename Input>
	  static void apply(const Input & in, L1::Program & p) {
      L1::Function* currentF = p.functions.back();

      L1::Number* n = new L1::Number(std::stoll(in.string()));

      parsed_items.push_back(n);
    }
  };

  template<> struct action <Label> {
    template<typename Input>
    static void apply(const Input & in, L1::Program & p) {
      L1::Label* l = new L1::Label(in.string());
      parsed_items.push_back(l);
    }
  };

  template<> struct action <Register> {
    template< typename Input >
    static void apply( const Input & in, L1::Program & p) {
      std::string s = in.string();
      Architecture::RegisterID reg_id = Architecture::reg_from_string(s);
      L1::Register* r = new L1::Register(reg_id);
      parsed_items.push_back(r);
    }
  };

  template<> struct action <MemoryLocation> {
    template<typename Input>
    static void apply(const Input & in, L1::Program & p) {
      auto n = parsed_items.back();
      L1::Number* real_n = (L1::Number*)n;
      parsed_items.pop_back();
      auto r = parsed_items.back();
      L1::Register* real_r = (L1::Register*)r;
      parsed_items.pop_back();
      L1::MemoryLocation* mem = new L1::MemoryLocation(real_r, real_n);
      parsed_items.push_back(mem);
    }
  };

  template<> struct action <operation> {
    template<typename Input>
    static void apply(const Input & in, L1::Program & p) {
      Architecture::OP opID = Architecture::OP_from_string(in.string());
      L1::Operation* op = new L1::Operation(opID);
      parsed_items.push_back(op);
    }
  };

  template<> struct action <cmp> {
    template<typename Input>
    static void apply(const Input & in, L1::Program & p) {
      Architecture::CompareOP cmpOPID = Architecture::cmpOP_from_string(in.string());
      L1::CmpOperation* cmpOP = new L1::CmpOperation(cmpOPID);
      parsed_items.push_back(cmpOP);
    }
  };

  template<> struct action <Instruction_return> {
    template<typename Input>
	  static void apply(const Input & in, L1::Program & p) {
      L1::Function* currentF = p.functions.back();
      int64_t num_locals = currentF->locals;
      int64_t num_args = currentF->arguments;
      L1::Number* num_locals_item = new L1::Number(num_locals);
      L1::Number* num_args_item = new L1::Number(num_args);
      L1::Instruction_return* i = new L1::Instruction_return(num_locals_item, num_args_item);
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_assignment> {
    template<typename Input>
	  static void apply(const Input & in, L1::Program & p) {
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
	  static void apply(const Input & in, L1::Program & p) {
      L1::Function* currentF = p.functions.back();

      auto op = parsed_items.back();
      parsed_items.pop_back();
      auto reg = parsed_items.back();
      parsed_items.pop_back();

      L1::NullItem* emptyThing = new L1::NullItem();
      L1::NullItem* emptyThing2 = new L1::NullItem();
      L1::NullItem* emptyThing3 = new L1::NullItem();
      auto test_name = typeid(*emptyThing).name();
      std::cout << test_name << std::endl;

      L1::Instruction_operation* i = new L1::Instruction_operation(reg, op, emptyThing, emptyThing2, emptyThing3);

      currentF->instructions.push_back(i);
    }
  };

  // operation with 2 items
  template<> struct action <Instruction_operation2> {
    template<typename Input>
	  static void apply(const Input & in, L1::Program & p) {
      L1::Function* currentF = p.functions.back();

      auto right = parsed_items.back();
      parsed_items.pop_back();
      auto op = parsed_items.back();
      parsed_items.pop_back();
      auto left = parsed_items.back();
      parsed_items.pop_back();

      L1::NullItem* emptyThing = new L1::NullItem();
      L1::NullItem* emptyThing2 = new L1::NullItem();

      L1::Instruction_operation* i = new L1::Instruction_operation(left, op, right, emptyThing, emptyThing2);

      currentF->instructions.push_back(i);
    }
  };

  // lea (3-ish items)
  template<> struct action <Instruction_operation3> {
    template<typename Input>
	  static void apply(const Input & in, L1::Program & p) {
      L1::Function* currentF = p.functions.back();

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

      L1::Instruction_operation* i = new L1::Instruction_operation(dst, op, first, second, factor);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_cjump> {
    template<typename Input>
    static void apply(const Input & in, L1::Program & p){
      L1::Function* currentF = p.functions.back();

      auto label = parsed_items.back();
      parsed_items.pop_back();
      auto right = parsed_items.back();
      parsed_items.pop_back();
      auto cmpOP = parsed_items.back();
      parsed_items.pop_back();
      auto left = parsed_items.back();
      parsed_items.pop_back();

      L1::Instruction_cjump* i = new L1::Instruction_cjump(left, cmpOP, right, label);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_save_cmp> {
    template<typename Input>
    static void apply(const Input & in, L1::Program & p){
      L1::Function* currentF = p.functions.back();

      auto right = parsed_items.back();
      parsed_items.pop_back();
      auto cmpOP = parsed_items.back();
      parsed_items.pop_back();
      auto left = parsed_items.back();
      parsed_items.pop_back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();

      L1::Instruction_save_cmp* i = new L1::Instruction_save_cmp(dst, left, cmpOP, right);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_label> {
    template<typename Input>
    static void apply(const Input & in, L1::Program & p){
      L1::Function* currentF = p.functions.back();

      L1::Label* label = new L1::Label(in.string());

      L1::Instruction_label* i = new L1::Instruction_label(label);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_goto> {
    template<typename Input>
    static void apply(const Input & in, L1::Program & p){
      L1::Function* currentF = p.functions.back();

      auto label = parsed_items.back();
      parsed_items.pop_back();

      L1::Instruction_goto* i = new L1::Instruction_goto(label);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_call> {
    template<typename Input>
	  static void apply(const Input & in, L1::Program & p){
      L1::Function* currentF = p.functions.back();

      auto num_args = parsed_items.back();
      parsed_items.pop_back();
      auto fn = parsed_items.back();
      parsed_items.pop_back();

      L1::Instruction_call* i = new L1::Instruction_call(fn, num_args);

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_call_print> {
    template<typename Input>
	  static void apply(const Input & in, L1::Program & p){
      L1::Function* currentF = p.functions.back();

      L1::Instruction_call_print* i = new L1::Instruction_call_print();

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_call_input> {
    template<typename Input>
	  static void apply(const Input & in, L1::Program & p){
      L1::Function* currentF = p.functions.back();

      L1::Instruction_call_input* i = new L1::Instruction_call_input();

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_call_allocate> {
    template< typename Input >
	  static void apply( const Input & in, L1::Program & p){
      L1::Function* currentF = p.functions.back();

      L1::Instruction_call_allocate* i = new L1::Instruction_call_allocate();

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action <Instruction_call_tensorError> {
    template<typename Input>
	  static void apply(const Input & in, L1::Program & p){
      L1::Function* currentF = p.functions.back();

      auto arg = parsed_items.back();
      parsed_items.pop_back();

      L1::Instruction_call_tensorError* i = new L1::Instruction_call_tensorError(arg);

      currentF->instructions.push_back(i);
    }
  };

  L1::Program parse_file (char *fileName){

    /* 
     * Check the grammar for some possible issues.
     */
    pegtl::analyze<grammar>();

    /*
     * Parse.
     */
    pegtl::file_input< > fileInput(fileName);
    L1::Program p;
    pegtl::parse<grammar, action>(fileInput, p);

    return p;
  }

}
