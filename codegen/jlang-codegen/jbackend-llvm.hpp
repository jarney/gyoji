#pragma once

#include <jlang-backend/jbackend.hpp>
#include <jlang-codegen/jsemantics.hpp>
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
  namespace codegen {
    namespace LLVM {

      using namespace JLang::frontend;
      using namespace JLang::backend;

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

class LLVMBinaryOperator {
public:
  typedef std::shared_ptr<LLVMBinaryOperator> ptr;
  LLVMBinaryOperator(llvm::IRBuilder<> &_builder);
  ~LLVMBinaryOperator();
  llvm::IRBuilder<> &builder;
  virtual std::string return_type() = 0;
  virtual llvm::Value *perform(llvm::Value *a, llvm::Value *b) = 0;
};

class LLVMBinaryOperatorTable {
public:
  // Returns the operator named 'name' that takes type_a and type_b as input types.
  LLVMBinaryOperator::ptr find_operator(std::string name, std::string type_a, std::string type_b);
  void register_operator(std::string name, std::string type_a, std::string type_b, LLVMBinaryOperator::ptr op);
};

class LLVMTranslationUnitVisitor
  : public JSemantics::Visitor<JSemantics::TranslationUnit> {
public:
  LLVMTranslationUnitVisitor();
  ~LLVMTranslationUnitVisitor();
  void initialize();
  void register_operator_builtins();
  void register_type_builtins();

  llvm::Value* codegen(JSemantics::ScopeBody &scope_block);
  
  void visit(JSemantics::ScopeBody &scope_body);
  void visit(JSemantics::TranslationUnit &visitable);
  void visit(JSemantics::GlobalVariableDefinition &global_variable_definition);
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
  LLVMBinaryOperatorTable binary_operators;

  // Binary operators are identified by the two types they accept
  // and give back the type they return.  Actually, this is a language
  // property, not a code-generation property, so this probably belongs
  // more in the semantic layer and not in the LLVM layer.
  //std::map<std::string, LLVMOperatorBinary::ptr> binary_operators;
  //std::map<std::string, LLVMOperatorUnary::ptr> unary_operators;

  llvm::Function *getFunction(std::string name);
  llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction,
                                           llvm::StringRef VarName);

};

    };
  };
};
