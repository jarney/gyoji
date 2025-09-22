#pragma once

#include <jlang-misc/pointers.hpp>
#include <jlang-mir.hpp>

namespace JLang::codegen {
  class CodeGeneratorLLVMContext;
  
  class CodeGeneratorLLVM {
  public:
    CodeGeneratorLLVM();
    ~CodeGeneratorLLVM();
    void initialize();
    int output(std::string filename);
  private:
    JLang::owned<CodeGeneratorLLVMContext> context;

  };

  void generate_code(JLang::mir::MIR & _mir);
};
