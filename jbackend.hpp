#pragma once

#include "jsyntax.hpp"

class JBackend {
public:
  JBackend();
  ~JBackend();
  virtual void process(ASTNode::ptr file) = 0;
};

/*
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

  class JBackendLLVM : public JBackend {
public:
  JBackendLLVM();
  ~JBackendLLVM();
  virtual void process(ASTNode::ptr file);

  std::unique_ptr<llvm::LLVMContext> TheContext;
  std::unique_ptr<llvm::IRBuilder<>> Builder;
  std::unique_ptr<llvm::Module> TheModule;
};
*/
