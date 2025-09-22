#pragma once

#include <jlang-misc/pointers.hpp>
#include <jlang-mir.hpp>

namespace JLang::codegen {
  class CodeGeneratorLLVMContext;

  /**
   * This is the public interface to the code generator.
   * It is careful not to pollute the namespace
   * by requiring inclusion of LLVM headers
   * so that the caller need know nothing at all
   * about how the code is actually generated.
   */
  class CodeGeneratorLLVM {
  public:
    CodeGeneratorLLVM();
    ~CodeGeneratorLLVM();
    void initialize();
    void generate(const JLang::mir::MIR & mir);
    int output(const std::string & filename);
  private:

    JLang::owned<CodeGeneratorLLVMContext> context;

  };

  void generate_code(JLang::mir::MIR & _mir, std::string & _filename);
};
