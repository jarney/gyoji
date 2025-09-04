#include "jbackend.hpp"

using namespace llvm;

JBackendLLVM::JBackendLLVM()
{
  // Open a new context and module.
  TheContext = std::make_unique<LLVMContext>();
  TheModule = std::make_unique<Module>("my cool jit", *TheContext);

  // Create a new builder for the module.
  Builder = std::make_unique<IRBuilder<>>(*TheContext);

}

JBackendLLVM::~JBackendLLVM()
{}

void JBackendLLVM::process(ASTNode::ptr file)
{

}

