#pragma once

#include <jlang-misc/pointers.hpp>
#include <jlang-mir.hpp>

// Hack, forward-declare llvm function.
namespace llvm {
  class Function;
  class AllocaInst;
  class StringRef;
};

namespace JLang::codegen {
  class CodeGeneratorLLVMContext;
  
  class CodeGeneratorLLVM {
  public:
    CodeGeneratorLLVM();
    ~CodeGeneratorLLVM();
    void initialize();

    void generate_function(const JLang::mir::Function & function);
    

    int output(std::string filename);
  private:

    llvm::Function * create_function(const JLang::mir::Function & function);
    llvm::AllocaInst *CreateEntryBlockAlloca(
                                             llvm::Function *TheFunction,
                                             const llvm::StringRef & VarName
                                             );
    JLang::owned<CodeGeneratorLLVMContext> context;

  };

  void generate_code(JLang::mir::MIR & _mir);
};
