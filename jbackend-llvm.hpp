#pragma once

#include "jbackend.hpp"
#include "jsemantics.hpp"
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

namespace JLang {
  namespace Backend {
    namespace LLVM {

class JBackendLLVM : public JBackend {
public:
  JBackendLLVM();
  ~JBackendLLVM();
  virtual int process(ASTNode::ptr file);
};


class LLVMType {
public:
  typedef std::shared_ptr<LLVMType> ptr;
  LLVMType(llvm::LLVMContext &_context);
  ~LLVMType();
  virtual llvm::Type *get_type() = 0;
  virtual llvm::Value *get_initializer_default() = 0;
  llvm::LLVMContext & context;
};

class LLVMTypeVoid : public LLVMType {
public:
  LLVMTypeVoid(llvm::LLVMContext &_context);;
  ~LLVMTypeVoid();
  llvm::Type *get_type();
  llvm::Value *get_initializer_default();
};

class LLVMTypeDouble : public LLVMType {
public:
  LLVMTypeDouble(llvm::LLVMContext &_context);
  ~LLVMTypeDouble();
  llvm::Type *get_type();
  llvm::Value *get_initializer_default();
};

      
class LLVMTranslationUnitVisitor
  : public JSemantics::Visitor<JSemantics::TranslationUnit> {
public:
  LLVMTranslationUnitVisitor();
  ~LLVMTranslationUnitVisitor();
  void initialize();
  
  void visit(JSemantics::TranslationUnit &visitable);
  void visit(JSemantics::GlobalVariableDefinition &visitable);
  void visit(JSemantics::FunctionDefinition &visitable);
  void visit(JSemantics::FunctionDeclaration &visitable);
  
  int output(std::string filename);
private:

  std::unique_ptr<llvm::LLVMContext> TheContext;
  std::unique_ptr<llvm::IRBuilder<>> Builder;
  std::unique_ptr<llvm::Module> TheModule;
  
  std::map<std::string, llvm::Value *> NamedValues;
  std::map<std::string, JSemantics::FunctionDeclaration::ptr> FunctionProtos;
  std::map<std::string, LLVMType::ptr> types;

  llvm::Function *getFunction(std::string name);
  llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction,
                                           llvm::StringRef VarName);

};

    };
  };
};
