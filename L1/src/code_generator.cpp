#include <string>
#include <iostream>
#include <fstream>
#include <L1.h>

#include <code_generator.h>

using namespace std;

namespace Generator{

  std::ofstream outputFile;
  
  void save_calle_saved_registers(std::ofstream file) {
    file << "  pushq %rbx" << std::endl;
    file << "  pushq %rbp" << std::endl;
    file << "  pushq %r12" << std::endl;
    file << "  pushq %r13" << std::endl;
    file << "  pushq %r14" << std::endl;
    file << "  pushq %r15" << std::endl;
    return;
  }

  void restore_calle_saved_registers(std::ofstream file) {
    file << "  popq %r15" << std::endl;
    file << "  popq %r14" << std::endl;
    file << "  popq %r13" << std::endl;
    file << "  popq %r12" << std::endl;
    file << "  popq %rbp" << std::endl;
    file << "  popq %rbx" << std::endl;
    return;
  }

  void generate_code(Program p) {

    /* 
     * Open the output file.
     */ 
    outputFile.open("prog.S");
   
    /* 
     * Generate target code
     */
    Generator::Assembly_visitor* generator;
    generator->visit(&p);

    /* 
     * Close the output file.
     */ 
    outputFile.close();
   
    return;
  }
}
