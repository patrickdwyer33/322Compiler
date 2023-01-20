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

using namespace pegtl;

namespace L1 {

  /* 
   * Tokens parsed
   */ 
  std::vector<Item *> parsed_items;

  /* 
   * Grammar rules from now on.
   */
  struct name:
    pegtl::seq<
      pegtl::plus< 
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >
        >
      >,
      pegtl::star<
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >,
          pegtl::digit
        >
      >
    > {};

  struct label:
    pegtl::seq<
      pegtl::one<':'>,
      name
    > {};

  struct number:
    pegtl::seq<
      pegtl::opt<
        pegtl::sor<
          pegtl::one< '-' >,
          pegtl::one< '+' >
        >
      >,
      pegtl::plus< 
        pegtl::digit
      >
    >{};

  struct argument_number:
    number {};

  struct local_number:
    number {} ;

  struct comment: 
    pegtl::disable< 
      TAOCPP_PEGTL_STRING( "//" ), 
      pegtl::until< pegtl::eolf > 
    > {};

  struct Register:
    pegtl::sor<
      register_rdi_rule,
      register_rax_rule,
      register_rsi_rule,
      register_rdx_rule,
      register_rcx_rule,
      register_r8_rule,
      register_r9_rule,
      register_rbx_rule,
      register_rbp_rule,
      register_r10_rule,
      register_r11_rule,
      register_r12_rule,
      register_r13_rule,
      register_r14_rule,
      register_r15_rule,
      stack_pointer_rule,
    >{};

  struct mem_access:
    pegtl::seq<
      TAOCPP_PEGTL_STRING("mem"),
      seps,
      x,
      seps,
      number
    >{};

  struct x:
    pegtl::sor<
      Register,
      stack_pointer_rule
    >{};

  struct u:
    pegtl::sor<
      Register,
      label
    >{};

  struct t:
    pegtl::sor<
      x,
      number
    >{};

  struct s:
    pegtl::sor<
      label,
      t
    >{};

  struct seps: 
    pegtl::star< 
      pegtl::sor< 
        pegtl::ascii::space, 
        comment 
      > 
    > {};

  struct plus_minus:
    pegtl::seq<
      pegtl::sor<
        pegtl::one<'+'>,
        pegtl::one<'-'>
        >,
      pegtl::one<'='>
    >{};

  struct multiply_and:
    pegtl::seq<
      pegtl::sor<
        pegtl::one<'*'>,
        pegtl::one<'&'>
        >,
      pegtl::one<'='>
    >{};

  struct aop:
    pegtl::seq<
      pegtl::sor<
        pegtl::one<'+'>,
        pegtl::one<'-'>,
        pegtl::one<'*'>,
        pegtl::one<'&'>
        >,
      pegtl::one<'='>
    >{};

  struct cmp:
    pegtl::sor<
      pegtl::one<'<'>,
      pegtl::one<'='>,
      pegtl::seq<
        pegtl::one<'<'>,
        pegtl::one<'='>
      >
    >{};

  struct sop:
    pegtl::seq<
      pegtl::sor<
        pegtl::seq<
          pegtl::one<'<'>
          pegtl::one<'<'>
        >,
        pegtl::seq<
          pegtl::one<'>'>
          pegtl::one<'>'>
        >
      >,
      pegtl::one<'='>
    >{};

  struct shifter:
    pegtl::sor<
      register_rcx_rule,
      number
    >{};

  /* 
   * Keywords.
   */
  struct str_return : TAOCPP_PEGTL_STRING( "return" ) {};
  struct str_arrow :
    pegtl::seq<
      pegtl::one<'<'>,
      pegtl::one<'-'>
    >{};
  struct str_rdi : TAOCPP_PEGTL_STRING( "rdi" ) {};
  struct str_rax : TAOCPP_PEGTL_STRING( "rax" ) {};
  struct str_rsi : TAOCPP_PEGTL_STRING( "rsi" ) {};
  struct str_rdx : TAOCPP_PEGTL_STRING( "rdx" ) {};
  struct str_rcx : TAOCPP_PEGTL_STRING( "rcx" ) {};
  struct str_r8 : TAOCPP_PEGTL_STRING( "r8" ) {};
  struct str_r9 : TAOCPP_PEGTL_STRING( "r9" ) {};
  struct str_rbx : TAOCPP_PEGTL_STRING( "rbx" ) {};
  struct str_rbp : TAOCPP_PEGTL_STRING( "rbp" ) {};
  struct str_r10 : TAOCPP_PEGTL_STRING( "r10" ) {};
  struct str_r11 : TAOCPP_PEGTL_STRING( "r11" ) {};
  struct str_r12 : TAOCPP_PEGTL_STRING( "r12" ) {};
  struct str_r13 : TAOCPP_PEGTL_STRING( "r13" ) {};
  struct str_r14 : TAOCPP_PEGTL_STRING( "r14" ) {};
  struct str_r15 : TAOCPP_PEGTL_STRING( "r15" ) {};
  struct str_rsp : TAOCPP_PEGTL_STRING( "rsp" ) {};

  struct cjump_str: TAOCPP_PEGTL_STRING( "cjump" ) {};
  struct goto_str: TAOCPP_PEGTL_STRING( "goto" ) {};
  struct call_str: TAOCPP_PEGTL_STRING( "call" ) {};
  struct print_str: TAOCPP_PEGTL_STRING( "print" ) {};
  struct input_str: TAOCPP_PEGTL_STRING( "input" ) {};
  struct allocate_str: TAOCPP_PEGTL_STRING( "allocate" ) {};
  struct tensorError_str: TAOCPP_PEGTL_STRING( "tensor-error" ) {};

  /* 
   * Rules.
   */

  struct function_name_rule:
    pegtl::seq<
      pegtl::one<'@'>,
      name
    > {};

  struct register_rdi_rule:
    str_rdi {};

  struct register_rax_rule:
    str_rax {};

  struct register_rsi_rule:
    str_rsi {};
  
  struct register_rdx_rule:
    str_rdx {};

  struct register_rcx_rule:
    str_rcx {};

  struct register_r8_rule:
    str_r8 {};

  struct register_r9_rule:
    str_r9 {};

  struct register_rbx_rule:
    str_rbx {};

  struct register_rbp_rule:
    str_rbp {};

  struct register_r10_rule:
    str_r9 {};
  
  struct register_r11_rule:
    str_r9 {};

  struct register_r12_rule:
    str_r9 {};

  struct register_r13_rule:
    str_r9 {};

  struct register_r14_rule:
    str_r9 {};

  struct register_r15_rule:
    str_r9 {};

  struct stack_pointer_rule:
    str_rsp {};

  struct save_cmp_rule:
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

  struct cjump_rule:
    pegtl::seq<
      cjump_str,
      seps,
      t,
      seps,
      cmp,
      seps,
      t,
      label
    >{};

  struct goto_rule:
    pegtl::seq<
      goto_str,
      seps,
      label
    >{};

  struct ret_label_rule:
    pegtl::seq<
      label
    >{};

  struct call_arg_number:
    number;

  struct call_fn_rule:
    pegtl::seq<
      call_str,
      seps,
      u,
      seps,
      call_arg_number
    >{};

  struct call_print_rule:
    pegtl::seq<
      call_str,
      seps,
      print_str,
      seps,
      call_arg_number
    >{};

  struct call_input_rule:
    pegtl::seq<
      call_str,
      seps,
      input_str,
      seps,
      call_arg_number
    >{};

  struct call_allocate_rule:
    pegtl::seq<
      call_str,
      seps,
      allocate_str,
      seps,
      call_arg_number
    >{};

  struct call_tensorError_rule:
    pegtl::seq<
      call_str,
      seps,
      tensorError_str,
      seps,
      call_arg_number
    >{};

  struct plus_plus_or_minus_minus_rule:
    pegtl::seq<
      Register,
      seps,
      pegtl::sor<
        pegtl::seq<
          pegtl::one<'+'>,
          pegtl::one<'+'>
        >,
        pegtl::seq<
          pegtl::one<'-'>,
          pegtl::one<'-'>
        >
      >
    >{};

  struct ampersand_op_rule:
    pegtl::seq<
      Register,
      seps,
      pegtl::one<'@'>,
      seps,
      Register,
      seps,
      Register,
      seps,
      number
    >{};

  struct aop_rule:
    pegtl::sor<
      pegtl::seq<
        Register,
        seps,
        aop,
        seps,
        t
      >,
      pegtl::seq<
        mem_access,
        seps,
        plus_minus,
        seps,
        t
      >,
      pegtl::seq<
        Register,
        seps,
        plus_minus,
        seps,
        mem_access
      >
    >{};

  struct sop_rule:
    pegtl::seq<
      register_rule,
      seps,
      sop,
      seps,
      shifter
    >{};

  struct Instruction_return_rule:
    pegtl::seq<
      str_return
    > { };

  struct Normal_instruction_assignment_rule:
    pegtl::seq<
      Register,
      seps,
      str_arrow,
      seps,
      s
    >{};

    struct Mem_read_instruction_assignment_rule:
      pegtl::seq<
        Regsiter,
        seps,
        str_arrow,
        seps,
        mem_access
      >{};

    struct Mem_write_instruction_assignment_rule:
      pegtl::seq<
        mem_access,
        seps,
        str_arrow,
        seps,
        s
      >{};

  struct Instruction_assignment_rule:
    pegtl::sor<
      Normal_instruction_assignment_rule,
      Mem_read_instruction_assignment_rule,
      Mem_write_instruction_assignment_rule
    > {};

  struct operation_rule:
    pegtl::sor<
      sop_rule,
      aop_rule,
      ampersand_op_rule,
      plus_plus_or_minus_minus_rule
    >{};

  struct call:
    pegtl::sor<
      call_fn,
      call_print,
      call_input,
      call_allocate,
      call_tensorError
    >{};

  struct call_rule:
    pegtl::seq<
      TAOCPP_PEGTL_STRING("mem"),
      seps,
      stack_pointer_rule,
      seps,
      TAOCPP_PEGTL_STRING("-8"),
      seps,
      call,
      seps,
      ret_label_rule
    >{};

  struct Instruction_rule:
    pegtl::sor<
      pegtl::seq< pegtl::at<Instruction_return_rule>, Instruction_return_rule>,
      pegtl::seq< pegtl::at<Instruction_assignment_rule>, Instruction_assignment_rule>,
      pegtl::seq< pegtl::at<Instruction_assignment_rule>, Instruction_assignment_rule>,
      pegtl::seq< pegtl::at<operation_rule>, operation_rule>,
      pegtl::seq< pegtl::at<call_rule>, call_rule>,
      pegtl::seq< pegtl::at<save_cmp_rule>, save_cmp_rule>,
      pegtl::seq< pegtl::at<cjump_rule>, cjump_rule>
      pegtl::seq< pegtl::at<goto_rule>, goto_rule>,
      pegtl::seq< pegtl::at<ret_label_rule>, ret_label_rule>
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
  template< typename Rule >
  struct action : pegtl::nothing< Rule > {};

  template<> struct action < function_name_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      if (p.entryPointLabel.empty()){
        p.entryPointLabel = in.string();
      } else {
        auto newF = new Function();
        newF->name = in.string();
        p.functions.push_back(newF);
      }
    }
  };

  template<> struct action < call_arg_number > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto num = Number(stoi(in.string()))
      parsed_items.push_back(num);
    }
  };

  template<> struct action < call_fn_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto num_args = parsed_items.back();
      parsed_items.pop_back();
      auto fn_name = parsed_items.back();
      parsed_items.pop_back();
      auto newInstruction = new Instruction_call(fn_name, num_args)
      currentF->instructions.push_back(newInstruction)
    }
  };

  template<> struct action < call_print_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto num_args = parsed_items.back();
      parsed_items.pop_back();
      auto newInstruction = new Instruction_call(LibraryCall::print)
      currentF->instructions.push_back(newInstruction)
    }
  };

  template<> struct action < call_input_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto num_args = parsed_items.back();
      parsed_items.pop_back();
      auto newInstruction = new Instruction_call(LibraryCall::input)
      currentF->instructions.push_back(newInstruction)
    }
  };

  template<> struct action < call_allocate_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto num_args = parsed_items.back();
      parsed_items.pop_back();
      auto newInstruction = new Instruction_call(LibraryCall::allocate)
      currentF->instructions.push_back(newInstruction)
    }
  };

  template<> struct action < call_tensorError_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto arg = parsed_items.back();
      parsed_items.pop_back();
      auto newInstruction = new Instruction_call(LibraryCall::tensor_error, arg)
      currentF->instructions.push_back(newInstruction)
    }
  };

  template<> struct action < argument_number > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      currentF->arguments = std::stoll(in.string());
    }
  };

  template<> struct action < local_number > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      currentF->locals = std::stoll(in.string());
    }
  };

  template<> struct action < str_return > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      auto i = new Instruction_ret();
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < register_rdi_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::rdi);
      parsed_items.push_back(r);
    }
  };

  template<> struct action < register_rax_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::rax);
      parsed_items.push_back(r);
    }
  };

  template<> struct action < register_rsi_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::rsi);
      parsed_items.push_back(r);
    }
  };

  template<> struct action < register_rdx_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::rdx);
      parsed_items.push_back(r);
    }
  };

  template<> struct action < register_r8_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::r8);
      parsed_items.push_back(r);
    }
  };

  template<> struct action < register_r9_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::r9);
      parsed_items.push_back(r);
    }
  };

  template<> struct action < register_rbx_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::rbx);
      parsed_items.push_back(r);
    }
  };

  template<> struct action < register_rbp_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::rbp);
      parsed_items.push_back(r);
    }
  };

  template<> struct action < register_r10_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::r10);
      parsed_items.push_back(r);
    }
  };

  template<> struct action < register_r11_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::r11);
      parsed_items.push_back(r);
    }
  };

  template<> struct action < register_r12_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::r12);
      parsed_items.push_back(r);
    }
  };

  template<> struct action < register_r13_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::r13);
      parsed_items.push_back(r);
    }
  };

  template<> struct action < register_r14_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::r14);
      parsed_items.push_back(r);
    }
  };

  template<> struct action < register_r15_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::r15);
      parsed_items.push_back(r);
    }
  };

  template<> struct action < stack_pointer_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = new Register(RegisterID::rsp);
      parsed_items.push_back(r);
    }
  };

  template<> struct action < save_cmp_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = parsed_items.back();
      parsed_items.pop_back();
      auto op = parsed_items.back();
      parsed_items.pop_back();
      auto newInstruction = new Instruction_save_cmp(r, op);
      auto currentF = p.functions.back();
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < cjump_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto loc = parsed_items.back();
      parsed_items.pop_back();
      auto op = parsed_items.back();
      parsed_items.pop_back();
      auto newInstruction = new Instruction_cjump(op, loc);
      auto currentF = p.functions.back();
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < goto_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto loc = parsed_items.back();
      parsed_items.pop_back();
      auto newInstruction = new Instruction_goto(loc);
      auto currentF = p.functions.back();
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_assignment_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){

      /* 
       * Fetch the current function.
       */ 
      auto currentF = p.functions.back();

      /*
       * Fetch the last two tokens parsed.
       */
      auto src = parsed_items.back();
      parsed_items.pop_back();
      auto dst = parsed_items.back();
      parsed_items.pop_back();

      /* 
       * Create the instruction.
       */ 
      auto i = new Instruction_assignment(dst, src);

      /* 
       * Add the just-created instruction to the current function.
       */ 
      currentF->instructions.push_back(i);
    }
  };

  Program parse_file (char *fileName){

    /* 
     * Check the grammar for some possible issues.
     */
    pegtl::analyze< grammar >();

    /*
     * Parse.
     */
    file_input< > fileInput(fileName);
    Program p;
    parse< grammar, action >(fileInput, p);

    return p;
  }

}
