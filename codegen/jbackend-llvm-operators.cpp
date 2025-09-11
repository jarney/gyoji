#include <jlang-codegen/jbackend-llvm-operators.hpp>

using namespace llvm::sys;
using namespace JSemantics;
using namespace JLang::Backend::LLVM;

LLVMBinaryOperator::ptr LLVMBinaryOperatorTable::find_operator(std::string name, std::string type_a, std::string type_b)
{
  return nullptr;
}

void LLVMBinaryOperatorTable::register_operator(std::string name, std::string type_a, std::string type_b, LLVMBinaryOperator::ptr op)
{}


LLVMBinaryOperator::LLVMBinaryOperator(llvm::IRBuilder<> &_builder)
  : builder(_builder)
{}
LLVMBinaryOperator::~LLVMBinaryOperator()
{}

LLVMBinaryOperatorAddDouble::LLVMBinaryOperatorAddDouble(llvm::IRBuilder<> &_builder)
  : LLVMBinaryOperator(_builder)
{}
LLVMBinaryOperatorAddDouble::~LLVMBinaryOperatorAddDouble()
{}

std::string LLVMBinaryOperatorAddDouble::return_type()
{
  return "double";
}

llvm::Value *LLVMBinaryOperatorAddDouble::perform(llvm::Value *a, llvm::Value *b)
{
  if (!b || !a)
    return nullptr;

  return builder.CreateFAdd(a, b, "addtmp");
}

