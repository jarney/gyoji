#include <jlang-codegen.hpp>
#include <stdio.h>

using namespace JLang::mir;

void JLang::codegen::generate_code(MIR & _mir, std::string & _filename)
{
  CodeGeneratorLLVM generator;
  generator.initialize();
  
  generator.generate(_mir);

  int rc = generator.output(_filename);
  
  fprintf(stderr, "Generating code\n");
}
