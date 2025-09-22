#pragma once

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"

namespace JLang::codegen {
  class CodeGeneratorLLVMContext {
  public:
    void initialize();
    void generate(const JLang::mir::MIR & mir);
    int output(const std::string & filename);
    
  private:
    JLang::owned<llvm::LLVMContext> TheContext;
    JLang::owned<llvm::IRBuilder<>> Builder;
    JLang::owned<llvm::Module> TheModule;
    std::map<std::string, llvm::Value *> NamedValues;

    void create_types(const JLang::mir::MIR & mir);
    void generate_function(const JLang::mir::Function & function);

    llvm::Function * create_function(const JLang::mir::Function & function);
    llvm::AllocaInst *CreateEntryBlockAlloca(
                                             llvm::Function *TheFunction,
                                             const llvm::StringRef & VarName
                                             );

  };
    
};
