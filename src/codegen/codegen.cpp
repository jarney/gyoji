#include <gyoji-codegen.hpp>
#include <gyoji-context.hpp>
#include <stdio.h>

using namespace Gyoji::mir;

void Gyoji::codegen::generate_code(
    const Gyoji::context::CompilerContext & _compiler_context,
    const MIR & _mir,
    const std::string & _filename
    )
{
    CodeGeneratorLLVM generator(_compiler_context, _mir);
    generator.initialize();  
    generator.generate();
    generator.output(_filename);
}
