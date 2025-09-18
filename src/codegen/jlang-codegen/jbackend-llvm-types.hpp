#pragma once
#include "jbackend-llvm.hpp"

namespace JLang {
  namespace codegen {
    namespace LLVM {

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

      
    };
  };
};
