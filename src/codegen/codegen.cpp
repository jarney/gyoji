#include <jlang-codegen.hpp>
#include <stdio.h>

using namespace JLang::mir;

void JLang::codegen::generate_code(MIR & _mir)
{
  CodeGeneratorLLVM llvm_visitor;
  llvm_visitor.initialize();
  
  int rc = llvm_visitor.output("output.o");
  
  fprintf(stderr, "Generating code\n");
}
