#include <jlang-codegen.hpp>
#include <stdio.h>

using namespace JLang::mir;

void JLang::codegen::generate_code(MIR & _mir)
{
  CodeGeneratorLLVM generator;
  generator.initialize();
  
  generator.generate(_mir);

  int rc = generator.output("output.o");
  
  fprintf(stderr, "Generating code\n");
}
