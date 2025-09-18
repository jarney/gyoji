#pragma once
#include "jbackend-llvm.hpp"

namespace JLang {
  namespace codegen {
    namespace LLVM {

class LLVMBinaryOperatorAddDouble : public LLVMBinaryOperator {
public:
  LLVMBinaryOperatorAddDouble(llvm::IRBuilder<> &_builder);
  ~LLVMBinaryOperatorAddDouble();
  std::string return_type();
  llvm::Value *perform(llvm::Value *a, llvm::Value *b);
};


    };
  };
};
