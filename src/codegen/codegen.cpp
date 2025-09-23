#include <jlang-codegen.hpp>
#include <stdio.h>

using namespace JLang::mir;

void JLang::codegen::generate_code(const MIR & _mir, const std::string & _filename)
{
    fprintf(stderr, "Generating code\n");
    CodeGeneratorLLVM generator(_mir);
    generator.initialize();  
    generator.generate();
    int rc = generator.output(_filename);
}
