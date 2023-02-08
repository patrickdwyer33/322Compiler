#include <string>
#include <iostream>
#include <fstream>
#include <L1.h>
#include <typeinfo>

#include <code_generator.h>

using namespace std;

namespace L1 {

  std::ofstream outputFile;

  L1::Assembly_visitor g;

  std::string NullItemName = "N2L18NullItemE";
  std::string NumberName = "N2L16NumberE";
  std::string LabelName = "N2L15LabelE";
  std::string RegisterName = "N2L18RegisterE";
  std::string MemoryLocationName = "N2L114MemoryLocationE";

  void generate_code(L1::Program &p) {

    /* 
     * Open the output file.
     */ 
    outputFile.open("prog.S");
   
    /* 
     * Generate target code
     */
    p.accept(&g);

    /* 
     * Close the output file.
     */ 
    outputFile.close();
   
    return;
  }

  void save_calle_saved_registers() {
    outputFile << "\t\tpushq %rbx" << std::endl;
    outputFile << "\t\tpushq %rbp" << std::endl;
    outputFile << "\t\tpushq %r12" << std::endl;
    outputFile << "\t\tpushq %r13" << std::endl;
    outputFile << "\t\tpushq %r14" << std::endl;
    outputFile << "\t\tpushq %r15" << std::endl;
    return;
  }

  void restore_calle_saved_registers() {
    outputFile << "\t\tpopq %r15" << std::endl;
    outputFile << "\t\tpopq %r14" << std::endl;
    outputFile << "\t\tpopq %r13" << std::endl;
    outputFile << "\t\tpopq %r12" << std::endl;
    outputFile << "\t\tpopq %rbp" << std::endl;
    outputFile << "\t\tpopq %rbx" << std::endl;
    return;
  }

  // w <- s
  void generate_assignment(L1::Register* r, L1::Number* n) {
    outputFile << "\t\tmovq $" << std::to_string(n->get()) << ", %" << Architecture::to_string(r->get()) << std::endl;
    return;
  }
  void generate_assignment(L1::Register* r, L1::Label* label) {
    outputFile << "\t\tmovq $_" << label->get().substr(1) << ", %" << Architecture::to_string(r->get()) << std::endl;
    return;
  }
  void generate_assignment(L1::Register* r1, L1::Register* r2) {
    outputFile << "\t\tmovq %" << Architecture::to_string(r2->get()) << ", %" << Architecture::to_string(r1->get()) << std::endl;
    return;
  }
  // w <- mem x M
  void generate_assignment(L1::Register* r, L1::MemoryLocation* mem) {
    auto mem_info = mem->get();
    auto reg = static_cast<L1::Register*>(mem_info[0]);
    auto offset = static_cast<L1::Number*>(mem_info[1]);
    outputFile << "\t\tmovq " << std::to_string(offset->get())<< "(%" << Architecture::to_string(reg->get()) << "), %" << Architecture::to_string(r->get()) << std::endl;
    return;
  }
  // mem x M <- s
  void generate_assignment(L1::MemoryLocation* mem, L1::Number* n) {
    auto mem_info = mem->get();
    auto reg = static_cast<L1::Register*>(mem_info[0]);
    auto offset = static_cast<L1::Number*>(mem_info[1]);
    outputFile << "\t\tmovq $" << std::to_string(n->get()) << ", " << std::to_string(offset->get())<< "(%" << Architecture::to_string(reg->get()) << ")" << std::endl;
    return;
  }
  void generate_assignment(L1::MemoryLocation* mem, L1::Label* label) {
    auto mem_info = mem->get();
    auto reg = static_cast<L1::Register*>(mem_info[0]);
    auto offset = static_cast<L1::Number*>(mem_info[1]);
    outputFile << "\t\tmovq $_" << label->get().substr(1) << ", " << std::to_string(offset->get())<< "(%" << Architecture::to_string(reg->get()) << ")" << std::endl;
    return;
  }
  void generate_assignment(L1::MemoryLocation* mem, L1::Register* r) {
    auto mem_info = mem->get();
    auto reg = static_cast<L1::Register*>(mem_info[0]);
    auto offset = static_cast<L1::Number*>(mem_info[1]);
    outputFile << "\t\tmovq %" << Architecture::to_string(r->get()) << ", " << std::to_string(offset->get())<< "(%" << Architecture::to_string(reg->get()) << ")" << std::endl;
    return;
  }
  // w aop t, w sop sx, w sop N
  void generate_op(L1::Register* r1, L1::Operation* op, L1::Register* r2, L1::NullItem* blank, L1::NullItem* blank2) {
    auto op_info = Architecture::get_op_instr(op->get());
    std::string instruction = std::get<0>(op_info);
    bool is_shift = std::get<1>(op_info);
    outputFile << "\t\t" << instruction << " %";
    if (is_shift) {
      outputFile << Architecture::get_eight_bit(r2->get());
    } else {
      outputFile << Architecture::to_string(r2->get());
    }
    outputFile << ", %" << Architecture::to_string(r1->get()) << std::endl;
    return;
  }
  void generate_op(L1::Register* r, L1::Operation* op, L1::Number* n, L1::NullItem* blank, L1::NullItem* blank2) {
    auto op_info = Architecture::get_op_instr(op->get());
    std::string instruction = std::get<0>(op_info);
    outputFile << "\t\t" << instruction << " $" << std::to_string(n->get());
    outputFile << ", %" << Architecture::to_string(r->get()) << std::endl;
    return;
  }
  // mem x M += t, mem x M -= t
  void generate_op(L1::MemoryLocation* mem, L1::Operation* op, L1::Register* r, L1::NullItem* blank, L1::NullItem* blank2) {
    auto op_info = Architecture::get_op_instr(op->get());
    auto mem_info = mem->get();
    auto reg = static_cast<L1::Register*>(mem_info[0]);
    auto offset = static_cast<L1::Number*>(mem_info[1]);
    std::string instruction = std::get<0>(op_info);
    outputFile << "\t\t" << instruction << " %" << Architecture::to_string(r->get()) << ", " << std::to_string(offset->get()) << "(%" << Architecture::to_string(reg->get()) << ")" << std::endl;
    return;
  }
  void generate_op(L1::MemoryLocation* mem, L1::Operation* op, L1::Number* n, L1::NullItem* blank, L1::NullItem* blank2) {
    auto op_info = Architecture::get_op_instr(op->get());
    auto mem_info = mem->get();
    auto reg = static_cast<L1::Register*>(mem_info[0]);
    auto offset = static_cast<L1::Number*>(mem_info[1]);
    std::string instruction = std::get<0>(op_info);
    outputFile << "\t\t" << instruction << " $" << std::to_string(n->get()) << ", " << std::to_string(offset->get()) << "(%" << Architecture::to_string(reg->get()) << ")" << std::endl;
    return;
  }
  // w += mem x M, w -= mem x M
  void generate_op(L1::Register* r, L1::Operation* op, L1::MemoryLocation* mem, L1::NullItem* blank, L1::NullItem* blank2) {
    auto op_info = Architecture::get_op_instr(op->get());
    auto mem_info = mem->get();
    auto reg = static_cast<L1::Register*>(mem_info[0]);
    auto offset = static_cast<L1::Number*>(mem_info[1]);
    std::string instruction = std::get<0>(op_info);
    outputFile << "\t\t" << instruction << " " << std::to_string(offset->get()) << "(%" << Architecture::to_string(reg->get()) << ")" << ", %" << Architecture::to_string(r->get()) << std::endl;
    return;
  }
  // w <- t cmp t
  void generate_save_cmp(L1::Register* dst, L1::Register* r1, L1::CmpOperation* cmpOP, L1::Register* r2) {
    outputFile << "\t\t" << "cmpq %" << Architecture::to_string(r2->get()) << ", %" << Architecture::to_string(r1->get()) << std::endl;
    outputFile << "\t\tset" << Architecture::get_cmp_instr(cmpOP->get()) << " %" << Architecture::get_eight_bit(dst->get()) << std::endl;
    outputFile << "\t\tmovzbq %" << Architecture::get_eight_bit(dst->get()) << ", %" << Architecture::to_string(dst->get()) << std::endl;
    return;
  }
  void generate_save_cmp(L1::Register* dst, L1::Register* r, L1::CmpOperation* cmpOP, L1::Number* n) {
    outputFile << "\t\t" << "cmpq $" << std::to_string(n->get()) << ", %" << Architecture::to_string(r->get()) << std::endl;
    outputFile << "\t\tset" << Architecture::get_cmp_instr(cmpOP->get()) << " %" << Architecture::get_eight_bit(dst->get()) << std::endl;
    outputFile << "\t\tmovzbq %" << Architecture::get_eight_bit(dst->get()) << ", %" << Architecture::to_string(dst->get()) << std::endl;
    return;
  }
  void generate_save_cmp(L1::Register* dst, L1::Number* n, L1::CmpOperation* cmpOP, L1::Register* r) {
    auto raw_op = cmpOP->get();
    switch (raw_op) {
      case Architecture::CompareOP::less_than:
        raw_op = Architecture::CompareOP::greater_than;
        break;
      case Architecture::CompareOP::less_than_or_equal:
        raw_op = Architecture::CompareOP::greater_than_or_equal;
        break;
      default:
        break;
    }
    auto new_cmpOP = new L1::CmpOperation(raw_op);
    generate_save_cmp(dst, r, new_cmpOP, n);
    return;
  }
  void generate_save_cmp(L1::Register* dst, L1::Number* n1, L1::CmpOperation* cmpOP, L1::Number* n2) {
    auto raw_op = cmpOP->get();
    bool comparison_result;
    switch (raw_op) {
      case Architecture::CompareOP::less_than:
        comparison_result = (n1->get() < n2->get());
        break;
      case Architecture::CompareOP::equal:
        comparison_result = (n1->get() == n2->get());
        break;
      case Architecture::CompareOP::less_than_or_equal:
        comparison_result = (n1->get() <= n2->get());
        break;
      default:
        std::cerr << "ERROR in generate_save_cmp n1 cmp n2" << std::endl;
    }
    int to_store = (int)comparison_result;
    outputFile << "\t\tmovq $" << std::to_string(to_store) << ", %" << Architecture::to_string(dst->get()) << std::endl;
    return;
  }
  // cjump t cmp t label
  void generate_cjump(L1::Register* r1, L1::CmpOperation* cmpOP, L1::Register* r2, L1::Label* label) {
    outputFile << "\t\tcmpq %" << Architecture::to_string(r2->get()) << ", %" << Architecture::to_string(r1->get()) << std::endl;
    outputFile << "\t\tj" << Architecture::get_cmp_instr(cmpOP->get()) << " _" << label->get().substr(1) << std::endl;
    return;
  }
  void generate_cjump(L1::Number* n, L1::CmpOperation* cmpOP, L1::Register* r, L1::Label* label) {
    auto raw_op = cmpOP->get();
    switch (raw_op) {
      case Architecture::CompareOP::less_than:
        raw_op = Architecture::CompareOP::greater_than;
        break;
      case Architecture::CompareOP::less_than_or_equal:
        raw_op = Architecture::CompareOP::greater_than_or_equal;
        break;
      default:
        break;
    }
    auto new_cmpOP = new L1::CmpOperation(raw_op);
    generate_cjump(r, new_cmpOP, n, label);
    return;
  }
  void generate_cjump(L1::Register* r, L1::CmpOperation* cmpOP, L1::Number* n, L1::Label* label) {
    outputFile << "\t\tcmpq $" << std::to_string(n->get()) << ", %" << Architecture::to_string(r->get()) << std::endl;
    outputFile << "\t\tj" << Architecture::get_cmp_instr(cmpOP->get()) << " _" << label->get().substr(1) << std::endl;
    return;
  }
  void generate_cjump(L1::Number* n1, L1::CmpOperation* cmpOP, L1::Number* n2, L1::Label* label) {
    auto raw_op = cmpOP->get();
    bool comparison_result;
    switch (raw_op) {
      case Architecture::CompareOP::less_than:
        comparison_result = (n1->get() < n2->get());
        break;
      case Architecture::CompareOP::equal:
        comparison_result = (n1->get() == n2->get());
        break;
      case Architecture::CompareOP::less_than_or_equal:
        comparison_result = (n1->get() <= n2->get());
        break;
      default:
        std::cerr << "ERROR in generate_save_cmp n1 cmp n2" << std::endl;
    }
    if (comparison_result) {
      outputFile << "\t\tjmp _" << label->get().substr(1) << std::endl;
    }
    return;
  }
  // label
  void generate_label(L1::Label* label) {
    outputFile << "_" << label->get().substr(1) << ":" << std::endl;
    return;
  }
  // goto label
  void generate_goto(L1::Label* label) {
    outputFile << "\t\tjmp _" << label->get().substr(1) << std::endl;
    return;
  }
  // return
  void generate_return(L1::Number* num_locals, L1::Number* num_args) {
    auto n = num_locals->get();
    auto args = num_args->get();
    if (n > 0 || args > 6) {
      outputFile << "\t\taddq $" << std::to_string(((int)n + (std::max(0, (int)args - 6))) * 8) << ", %rsp" << std::endl;
    }
    outputFile << "\t\tretq" << std::endl;
    return;
  }
  // call u N
  void generate_call(L1::Register* r, L1::Number* n) {
    outputFile << "\t\tsubq $" << std::to_string((8 + (8 * std::max(0, ((int)n->get() - 6))))) << ", %rsp" << std::endl;
    outputFile << "\t\tjmp *%" << Architecture::to_string(r->get()) << std::endl;
    return;
  }
  void generate_call(L1::Label* label, L1::Number* n) {
    outputFile << "\t\tsubq $" << std::to_string((8 + (8 * std::max(0, ((int)n->get() - 6))))) << ", %rsp" << std::endl;
    outputFile << "\t\tjmp _" << label->get().substr(1) << std::endl;
    return;
  }
  // call print 1
  void generate_call_print() {
    outputFile << "\t\tcall print" << std::endl;
    return;
  }
  // call input 0
  void generate_call_input() {
    outputFile << "\t\tcall input" << std::endl;
    return;
  }
  // call allocate 2
  void generate_call_allocate() {
    outputFile << "\t\tcall allocate" << std::endl;
    return;
  }
  // call tensor-error F
  void generate_call_tensorError(L1::Number* n) {
    auto arg = n->get();
    outputFile << "\t\tcall ";
    switch (arg) {
      case 1:
        outputFile << "array_tensor_error_null";
        break;
      case 3:
        outputFile << "array_error";
        break;
      case 4:
        outputFile << "tensor_error";
        break;
    }
    outputFile << std::endl;
    return;
  }
  // w ++, w --
  void generate_op(L1::Register* r, L1::Operation* op, L1::NullItem* blank, L1::NullItem* blank2, L1::NullItem* blank3) {
    outputFile << "\t\t" << std::get<0>(Architecture::get_op_instr(op->get())) << " %" << Architecture::to_string(r->get()) << std::endl;
    return;
  }
  // w @ w w E
  void generate_op(L1::Register* dst, L1::Operation* op, L1::Register* first, L1::Register* second, L1::Number* factor) {
    outputFile << "\t\tlea (%" << Architecture::to_string(first->get()) << ", %" << Architecture::to_string(second->get()) << ", " << std::to_string(factor->get()) << "), %" << Architecture::to_string(dst->get()) << std::endl;
    return;
  }

  void Assembly_visitor::visit(L1::Instruction_return* i) {
    auto instr_data = i->get();
    auto type_name = typeid(*instr_data[0]).name();
    if (type_name != NumberName) {
      std::cerr << "ERROR in visit Instruction_return."<< std::endl;
    }
    L1::Number* n = (L1::Number*)instr_data[0];
    L1::Number* num_args = (L1::Number*)instr_data[1];
    generate_return(n, num_args);
    return;
  }
  void Assembly_visitor::visit(L1::Instruction_assignment* i) {
    auto instr_data = i->get();
    auto type_name_1 = typeid(*instr_data[0]).name();
    auto type_name_2 = typeid(*instr_data[1]).name();
    if (type_name_1 == RegisterName && type_name_2 == NumberName) {
      L1::Register* r = static_cast<L1::Register*>(instr_data[0]);
      L1::Number* n = static_cast<L1::Number*>(instr_data[1]);
      generate_assignment(r, n);
    } else if (type_name_1 == RegisterName && type_name_2 == LabelName) {
      L1::Register* r = static_cast<L1::Register*>(instr_data[0]);
      L1::Label* label = static_cast<L1::Label*>(instr_data[1]);
      generate_assignment(r, label);
    } else if (type_name_1 == RegisterName && type_name_2 == RegisterName) {
      L1::Register* r1 = static_cast<L1::Register*>(instr_data[0]);
      L1::Register* r2 = static_cast<L1::Register*>(instr_data[1]);
      generate_assignment(r1, r2);
    } else if (type_name_1 == RegisterName && type_name_2 == MemoryLocationName) {
      L1::Register* r = static_cast<L1::Register*>(instr_data[0]);
      L1::MemoryLocation* m = static_cast<L1::MemoryLocation*>(instr_data[1]);
      generate_assignment(r, m);
    } else if (type_name_1 == MemoryLocationName && type_name_2 == NumberName) {
      L1::MemoryLocation* m = static_cast<L1::MemoryLocation*>(instr_data[0]);
      L1::Number* n = static_cast<L1::Number*>(instr_data[1]);
      generate_assignment(m, n);
    } else if (type_name_1 == MemoryLocationName && type_name_2 == LabelName) {
      L1::MemoryLocation* m = static_cast<L1::MemoryLocation*>(instr_data[0]);
      L1::Label* label = static_cast<L1::Label*>(instr_data[1]);
      generate_assignment(m, label);
    } else if (type_name_1 == MemoryLocationName && type_name_2 == RegisterName) {
      L1::MemoryLocation* m = static_cast<L1::MemoryLocation*>(instr_data[0]);
      L1::Register* r = static_cast<L1::Register*>(instr_data[1]);
      generate_assignment(m, r);
    } else {
      std::cerr << "ERROR in visit Instruction_assignment" << std::endl;
    }

    return;
  }
  void Assembly_visitor::visit(L1::Instruction_operation* i) {
    auto instr_data = i->get();
    auto type_name_1 = typeid(*instr_data[0]).name();
    auto type_name_2 = typeid(*instr_data[1]).name();
    auto type_name_3 = typeid(*instr_data[2]).name();
    auto type_name_4 = typeid(*instr_data[3]).name();
    auto type_name_5 = typeid(*instr_data[4]).name();
    if (type_name_5 != NullItemName) {
      L1::Register* dst = static_cast<L1::Register*>(instr_data[0]);
      L1::Operation* op = static_cast<L1::Operation*>(instr_data[1]);
      L1::Register* r1 = static_cast<L1::Register*>(instr_data[2]);
      L1::Register* r2 = static_cast<L1::Register*>(instr_data[3]);
      L1::Number* n = static_cast<L1::Number*>(instr_data[4]);
      generate_op(dst, op, r1, r2, n);
    } else if (type_name_1 == RegisterName && type_name_3 == RegisterName ) {
      L1::Register* r1 = static_cast<L1::Register*>(instr_data[0]);
      L1::Operation* op = static_cast<L1::Operation*>(instr_data[1]);
      L1::Register* r2 = static_cast<L1::Register*>(instr_data[2]);
      L1::NullItem* empty1 = static_cast<L1::NullItem*>(instr_data[3]);
      L1::NullItem* empty2 = static_cast<L1::NullItem*>(instr_data[4]);
      generate_op(r1, op, r2, empty1, empty2);
    } else if (type_name_1 == RegisterName && type_name_3 == NumberName) {
      L1::Register* r = static_cast<L1::Register*>(instr_data[0]);
      L1::Operation* op = static_cast<L1::Operation*>(instr_data[1]);
      L1::Number* n = static_cast<L1::Number*>(instr_data[2]);
      L1::NullItem* empty1 = static_cast<L1::NullItem*>(instr_data[3]);
      L1::NullItem* empty2 = static_cast<L1::NullItem*>(instr_data[4]);
      generate_op(r, op, n, empty1, empty2);
    } else if (type_name_1 == MemoryLocationName && type_name_3 == RegisterName) {
      L1::MemoryLocation* m = static_cast<L1::MemoryLocation*>(instr_data[0]);
      L1::Operation* op = static_cast<L1::Operation*>(instr_data[1]);
      L1::Register* r = static_cast<L1::Register*>(instr_data[2]);
      L1::NullItem* empty1 = static_cast<L1::NullItem*>(instr_data[3]);
      L1::NullItem* empty2 = static_cast<L1::NullItem*>(instr_data[4]);
      generate_op(m, op, r, empty1, empty2);
    } else if (type_name_1 == MemoryLocationName && type_name_3 == NumberName) {
      L1::MemoryLocation* m = static_cast<L1::MemoryLocation*>(instr_data[0]);
      L1::Operation* op = static_cast<L1::Operation*>(instr_data[1]);
      L1::Number* n = static_cast<L1::Number*>(instr_data[2]);
      L1::NullItem* empty1 = static_cast<L1::NullItem*>(instr_data[3]);
      L1::NullItem* empty2 = static_cast<L1::NullItem*>(instr_data[4]);
      generate_op(m, op, n, empty1, empty2);
    } else if (type_name_1 == RegisterName && type_name_3 == MemoryLocationName) {
      L1::Register* r = static_cast<L1::Register*>(instr_data[0]);
      L1::Operation* op = static_cast<L1::Operation*>(instr_data[1]);
      L1::MemoryLocation* m = static_cast<L1::MemoryLocation*>(instr_data[2]);
      L1::NullItem* empty1 = static_cast<L1::NullItem*>(instr_data[3]);
      L1::NullItem* empty2 = static_cast<L1::NullItem*>(instr_data[4]);
      generate_op(r, op, m, empty1, empty2);
    } else {
      L1::Register* r = static_cast<L1::Register*>(instr_data[0]);
      L1::Operation* op = static_cast<L1::Operation*>(instr_data[1]);
      L1::NullItem* empty1 = static_cast<L1::NullItem*>(instr_data[2]);
      L1::NullItem* empty2 = static_cast<L1::NullItem*>(instr_data[3]);
      L1::NullItem* empty3 = static_cast<L1::NullItem*>(instr_data[4]);
      generate_op(r, op, empty1, empty2, empty3);
    } 
    return;
  }
  void Assembly_visitor::visit(L1::Instruction_save_cmp* i) {
    auto instr_data = i->get();
    auto type_name_2 = typeid(*instr_data[1]).name();
    auto type_name_4 = typeid(*instr_data[3]).name();
    L1::Register* first = static_cast<L1::Register*>(instr_data[0]);
    L1::CmpOperation* third = (L1::CmpOperation*)instr_data[2];
    if (type_name_2 == RegisterName && type_name_4 == RegisterName) {
      L1::Register* second = static_cast<L1::Register*>(instr_data[1]);
      L1::Register* fourth = static_cast<L1::Register*>(instr_data[3]);
      generate_save_cmp(first, second, third, fourth);
    } else if (type_name_2 == RegisterName && type_name_4 == NumberName) {
      L1::Register* second = static_cast<L1::Register*>(instr_data[1]);
      L1::Number* fourth = static_cast<L1::Number*>(instr_data[3]);
      generate_save_cmp(first, second, third, fourth);
    } else if (type_name_2 == NumberName && type_name_4 == RegisterName) {
      L1::Number* second = static_cast<L1::Number*>(instr_data[1]);
      L1::Register* fourth = static_cast<L1::Register*>(instr_data[3]);
      generate_save_cmp(first, second, third, fourth);
    } else {
      L1::Number* second = static_cast<L1::Number*>(instr_data[1]);
      L1::Number* fourth = static_cast<L1::Number*>(instr_data[3]);
      generate_save_cmp(first, second, third, fourth);
    }
    
    return;
  }
  void Assembly_visitor::visit(L1::Instruction_cjump* i) {
    auto instr_data = i->get();
    auto type_name_1 = typeid(*instr_data[0]).name();
    auto type_name_3 = typeid(*instr_data[2]).name();
    L1::CmpOperation* second = static_cast<L1::CmpOperation*>(instr_data[1]);
    L1::Label* fourth = static_cast<L1::Label*>(instr_data[3]);
    if (type_name_1 == RegisterName && type_name_3 == RegisterName) {
      L1::Register* first = static_cast<L1::Register*>(instr_data[0]);
      L1::Register* third = static_cast<L1::Register*>(instr_data[2]);
      generate_cjump(first, second, third, fourth);
    } else if (type_name_1 == RegisterName && type_name_3 == NumberName) {
      L1::Register* first = static_cast<L1::Register*>(instr_data[0]);
      L1::Number* third = static_cast<L1::Number*>(instr_data[2]);
      generate_cjump(first, second, third, fourth);
    } else if (type_name_1 == NumberName && type_name_3 == RegisterName) {
      L1::Number* first = static_cast<L1::Number*>(instr_data[0]);
      L1::Register* third = static_cast<L1::Register*>(instr_data[2]);
      generate_cjump(first, second, third, fourth);
    } else {
      L1::Number* first = static_cast<L1::Number*>(instr_data[0]);
      L1::Number* third = static_cast<L1::Number*>(instr_data[2]);
      generate_cjump(first, second, third, fourth);
    }
    return;
  }
  void Assembly_visitor::visit(L1::Instruction_label* i) {
    auto instr_data = i->get();
    L1::Label* label = (L1::Label*)instr_data;
    generate_label(label);
    return;
  }
  void Assembly_visitor::visit(L1::Instruction_goto* i) {
    auto instr_data = i->get();
    L1::Label* label = (L1::Label*)instr_data;
    generate_goto(label);
    return;
  }
  void Assembly_visitor::visit(L1::Instruction_call* i) {
    auto instr_data = i->get();
    L1::Number* second = static_cast<L1::Number*>(instr_data[1]);
    auto type_name_1 = typeid(*instr_data[0]).name();
    if (type_name_1 == RegisterName) {
      L1::Register* first = static_cast<L1::Register*>(instr_data[0]);
      generate_call(first, second);
    } else {
      L1::Label* first = static_cast<L1::Label*>(instr_data[0]);
      generate_call(first, second);
    }
    return;
  }
  void Assembly_visitor::visit(L1::Instruction_call_print* i) {
    generate_call_print();
    return;
  }
  void Assembly_visitor::visit(L1::Instruction_call_input* i) {
    generate_call_input();
    return;
  }
  void Assembly_visitor::visit(L1::Instruction_call_allocate* i) {
    generate_call_allocate();
    return;
  }
  void Assembly_visitor::visit(L1::Instruction_call_tensorError* i) {
    auto instr_data = i->get();
    L1::Number* first = (L1::Number*)instr_data;
    generate_call_tensorError(first);
    return;
  }

  void Assembly_visitor::visit(L1::Program* p) {
    outputFile << "\t\t.text" << std::endl << "\t\t.globl go" << std::endl << "go:" << std::endl;
    save_calle_saved_registers();
    outputFile << "\t\tcall _" << p->entryPointLabel.substr(1) << std::endl;
    restore_calle_saved_registers();
    outputFile << "\t\tretq" << std::endl;
    for (auto f: p->functions) {
      f->accept(&g);
    }
    return;
  }

  void Assembly_visitor::visit(L1::Function* fn) {
    outputFile << "_" << fn->name.substr(1) << ":" << std::endl;
    auto num_locals = fn->locals;
    if (num_locals > 0) {
      outputFile << "\t\tsubq $" << std::to_string(((int)num_locals * 8)) << ", %rsp" << std::endl;
    }
    for (auto i: fn->instructions) {
      i->accept(&g);
    }
    return;
  }

}
