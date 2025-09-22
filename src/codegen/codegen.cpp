#include <jlang-codegen.hpp>
#include <stdio.h>

using namespace JLang::mir;

void JLang::codegen::generate_code(MIR & _mir)
{
  CodeGeneratorLLVM generator;
  generator.initialize();
  

  const Functions & functions = _mir.get_functions();
  for (auto const & function : functions.get_functions()) {
    fprintf(stderr, "Generating for function %s\n", function->get_name().c_str());
    generator.generate_function(*function);
  }

  int rc = generator.output("output.o");
  
  fprintf(stderr, "Generating code\n");
}
