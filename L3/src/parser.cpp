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

  struct arg_var_barrier:
    pegtl::seq<pegtl::one<'('>> {};

  struct no_args:
    pegtl::seq<
      arg_var_barrier,
      seps,
      pegtl::one<')'>
    > {};

  struct one_arg:
    pegtl::seq<
      arg_var_barrier,
      seps,
      t,
      seps,
      pegtl::one<')'>
    > {};

  struct many_args:
    pegtl::seq<
      arg_var_barrier,
      seps,
      t,
      pegtl::plus<
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
      pegtl::seq<pegtl::at<many_args>, many_args>,
      pegtl::seq<pegtl::at<one_arg>, one_arg>,
      pegtl::seq<pegtl::at<no_args>, no_args>
    > {};

  struct no_vars:
    pegtl::seq<
      arg_var_barrier,
      seps,
      pegtl::one<')'>
    > {};

  struct one_var:
    pegtl::seq<
      arg_var_barrier,
      seps,
      var,
      seps,
      pegtl::one<')'>
    > {};

  struct many_vars:
    pegtl::seq<
      arg_var_barrier,
      seps,
      var,
      pegtl::plus<
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
      pegtl::seq<pegtl::at<many_vars>, many_vars>,
      pegtl::seq<pegtl::at<one_var>, one_var>,
      pegtl::seq<pegtl::at<no_vars>, no_vars>
    > {};

  struct callee:
    pegtl::sor<
      u,
      TAOCPP_PEGTL_STRING("print"),
      TAOCPP_PEGTL_STRING("allocate"),
      TAOCPP_PEGTL_STRING("input"),
      TAOCPP_PEGTL_STRING("tensor-error")
    > {};

  struct Instruction_label:
    pegtl::seq<label> {};

  struct return_val:
    pegtl::seq<t> {};

  struct Instruction_return : 
    pegtl::sor<
      pegtl::seq<
        TAOCPP_PEGTL_STRING("return"),
        seps,
        return_val
      >,
      TAOCPP_PEGTL_STRING("return")
    > {};

  struct branch_val:
    pegtl::seq<t> {};

  struct Instruction_branch:
    pegtl::sor<
      pegtl::seq<
        TAOCPP_PEGTL_STRING("br"),
        seps,
        branch_val,
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

  ////// MAKE SURE THAT WEIRD COLORING HERE IS JUST A FORMATTING ISSUE

  struct store_item:
    TAOCPP_PEGTL_STRING("store") {};

  struct left_hand:
    pegtl::sor<
      pegtl::seq<
        store_item,
        seps,
        var
      >,
      var
    > {};

  struct load_item:
    TAOCPP_PEGTL_STRING("load") {};

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
        load_item,
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

  struct arrow:
    str_arrow {};


  struct Instruction_assignment:
    pegtl::seq<
      left_hand,
      seps,
      arrow,
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
    > {};

  struct Functions_rule:
    pegtl::plus<
      seps,
      Function_rule,
      seps
    > {};

  struct entry_point_rule:
    pegtl::seq<
      Functions_rule
    > {};

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

  template<> struct action <var> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      //Function* curF = p.functions.back();
      L3::Variable* var = new L3::Variable(in.string());
      parsed_items.push_back(var);
    }
  };

  template<> struct action <label> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::Label* var = new L3::Label(in.string());
      parsed_items.push_back(var);
    }
  };

  template<> struct action <fn_name> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::Fn_Name_Item* fn_name = new L3::Fn_Name_Item(in.string().substr(1));
      parsed_items.push_back(fn_name);
    }
  };

  template<> struct action <Number> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::Number* n = new L3::Number(std::stoll(in.string()));
      parsed_items.push_back(n);
    }
  };

  template<> struct action <cmp> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::CmpOP* op = new L3::CmpOP(in.string());
      parsed_items.push_back(op);
    }
  };

  template<> struct action <op> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::OP* op = new L3::OP(in.string());
      parsed_items.push_back(op);
    }
  };

  template<> struct action <arg_var_barrier> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::Barrier* bar = new L3::Barrier();
      parsed_items.push_back(bar);
    }
  };

  template<> struct action <fn_declaration> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::Function* newF = new L3::Function();
      // store num args last when vars fires
      L3::Item* last = parsed_items.back();
      L3::Barrier* bar = dynamic_cast<L3::Barrier*>(last);
      uint64_t num_vars = 0;
      std::vector<L3::Item*> vars_rev;
      while (bar == NULL) {
        num_vars++;
        parsed_items.pop_back();
        vars_rev.push_back(last);
        last = parsed_items.back();
        bar = dynamic_cast<L3::Barrier*>(last);
      }
      parsed_items.pop_back();
      // reverse em
      while (!vars_rev.empty()) {
        L3::Item* asdf = vars_rev.back();
        vars_rev.pop_back();
        newF->vars.push_back(asdf);
      }
      L3::Item* fn_name = parsed_items.back();
      parsed_items.pop_back();
      newF->name = fn_name->s;
      p.functions.push_back(newF);
    }
  };

  template<> struct action <return_val> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::Item* val = parsed_items.back();
      parsed_items.pop_back();
      L3::Return_val* ret_val = new L3::Return_val(val);
      parsed_items.push_back(ret_val);
    }
  };

  template<> struct action <Instruction_return> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::Function* curF = p.functions.back();
      if (parsed_items.empty()) {
        L3::Instruction_return* instr = new L3::Instruction_return();
        curF->instructions.push_back(instr);
        return;
      }
      L3::Item* back = parsed_items.back();
      L3::Return_val* ret = dynamic_cast<L3::Return_val*>(back);
      if (ret != NULL) {
        parsed_items.pop_back();
        L3::Instruction_return* instr = new L3::Instruction_return(ret->val);
        curF->instructions.push_back(instr);
        return;
      }
      L3::Instruction_return* instr = new L3::Instruction_return();
      curF->instructions.push_back(instr);
    }
  };

  template<> struct action <Instruction_label> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::Item* label = parsed_items.back();
      parsed_items.pop_back();
      L3::Function* curF = p.functions.back();
      L3::Label* label_item = static_cast<L3::Label*>(label);
      label_item->s = label_item->s + "_" + curF->name;
      L3::Instruction_label* instr = new L3::Instruction_label(label_item);
      curF->instructions.push_back(instr);
    }
  };

  template<> struct action <branch_val> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::Item* val = parsed_items.back();
      parsed_items.pop_back();
      L3::Branch_val* br_val = new L3::Branch_val(val);
      parsed_items.push_back(br_val);
    }
  };

  template<> struct action <Instruction_branch> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::Function* curF = p.functions.back();
      L3::Item* label = parsed_items.back();
      parsed_items.pop_back();
      if (parsed_items.empty()) {
        L3::Instruction_branch* instr = new L3::Instruction_branch(label);
        curF->instructions.push_back(instr);
        return;
      }
      L3::Item* back = parsed_items.back();
      L3::Branch_val* br = dynamic_cast<L3::Branch_val*>(back);
      if (br != NULL) {
        parsed_items.pop_back();
        L3::Instruction_branch* instr = new L3::Instruction_branch(label, br->val);
        curF->instructions.push_back(instr);
      }
      L3::Instruction_branch* instr = new L3::Instruction_branch(label);
      curF->instructions.push_back(instr);
    }
  };

  template<> struct action <Instruction_call> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::Function* curF = p.functions.back();
      if (parsed_items.empty()) {
        std::vector<L3::Item*> args = {};
        L3::Instruction_call* instr = new L3::Instruction_call(0, args, "print");
        curF->instructions.push_back(instr);
        return;
      }
      L3::Item* last = parsed_items.back();
      L3::Barrier* bar = dynamic_cast<L3::Barrier*>(last);
      uint64_t num_args = 0;
      std::vector<L3::Item*> args_rev;
      while (bar == NULL) {
        num_args++;
        parsed_items.pop_back();
        args_rev.push_back(last);
        last = parsed_items.back();
        bar = dynamic_cast<L3::Barrier*>(last);
      }
      parsed_items.pop_back();
      // reverse em
      std::vector<L3::Item*> args;
      while (!args_rev.empty()) {
        L3::Item* asdf = args_rev.back();
        args_rev.pop_back();
        args.push_back(asdf);
      }
      L3::Instruction_call* instr = new L3::Instruction_call(num_args, args, "print");
      curF->instructions.push_back(instr);
    }
  };

  template<> struct action <Instruction_call_item> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      if (parsed_items.empty()) {
        std::vector<L3::Item*> args = {};
        L3::Call_item* call_item = new L3::Call_item(0, args, "print");
        parsed_items.push_back(call_item);
        return;
      }
      L3::Item* last = parsed_items.back();
      parsed_items.pop_back();
      L3::Barrier* bar = dynamic_cast<L3::Barrier*>(last);
      uint64_t num_args = 0;
      std::vector<L3::Item*> args_rev;
      while (bar == NULL) {
        num_args++;
        parsed_items.pop_back();
        args_rev.push_back(last);
        last = parsed_items.back();
        bar = dynamic_cast<L3::Barrier*>(last);
      }
      parsed_items.pop_back();
      // reverse em
      std::vector<L3::Item*> args;
      while (!args_rev.empty()) {
        L3::Item* asdf = args_rev.back();
        args_rev.pop_back();
        args.push_back(asdf);
      }
      L3::Call_item* call = new L3::Call_item(num_args, args, "print");
      parsed_items.push_back(call);
    }
  };

  template<> struct action <store_item> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::Store* store_item = new L3::Store();
      parsed_items.pop_back();
    }
  };

  template<> struct action <load_item> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::Load* load_item = new L3::Load();
      parsed_items.pop_back();
    }
  };

  template<> struct action <arrow> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::Arrow* arrow = new L3::Arrow();
      parsed_items.pop_back();
    }
  };

  template<> struct action <Instruction_assignment> {
    template<typename Input>
	  static void apply(const Input & in, L3::Program & p) {
      L3::Function* curF = p.functions.back();
      L3::Item* cur = parsed_items.back();
      L3::Arrow* arrow = dynamic_cast<L3::Arrow*>(cur);
      bool is_store;
      bool is_load;
      L3::Instruction_call* call_instr;
      std::vector<L3::Item*> right_stuff;
      while (arrow == NULL) {
        parsed_items.pop_back();
        L3::Call_item* call = dynamic_cast<L3::Call_item*>(cur);
        if (call != NULL) {
          call_instr = new L3::Instruction_call(call->num_args, call->args, "print");
          continue;
        }
        L3::Store* store = dynamic_cast<L3::Store*>(cur);
        if (store != NULL) {
          is_store = true;
          continue;
        }
        L3::Load* load = dynamic_cast<L3::Load*>(cur);
        if (load != NULL) {
          is_load = true;
          continue;
        }
        right_stuff.push_back(cur);
        cur = parsed_items.back();
        arrow = dynamic_cast<L3::Arrow*>(cur);
      }
      parsed_items.pop_back();
      L3::Item* left = parsed_items.back();
      parsed_items.pop_back();
      L3::Instruction_assignment* instr = new L3::Instruction_assignment(left, right_stuff);
      instr->is_store = is_store;
      instr->is_load = is_load;
      instr->call_instr = call_instr;
      curF->instructions.push_back(instr);
    }
  };

  L3::Program parse_file (char *fileName){

    /* 
     * Check the grammar for some possible issues.
     */
    pegtl::analyze<grammar>();

    /*
     * Parse.
     */
    pegtl::file_input< > fileInput(fileName);
    L3::Program p;
    pegtl::parse<grammar, action>(fileInput, p);

    return p;
  }

}
