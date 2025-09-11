#include <jlang-codegen/jbackend-llvm-types.hpp>

using namespace llvm::sys;
using namespace JSemantics;
using namespace JLang::Backend::LLVM;

LLVMType::LLVMType(llvm::LLVMContext & _context)
  : context(_context)
{}
LLVMType::~LLVMType()
{}

LLVMTypeVoid::LLVMTypeVoid(llvm::LLVMContext & _context)
  : LLVMType(_context)
{}
LLVMTypeVoid::~LLVMTypeVoid()
{}
llvm::Type *LLVMTypeVoid::get_type()
{
  return llvm::Type::getVoidTy(context);
}
llvm::Value *LLVMTypeVoid::get_initializer_default()
{
  return llvm::UndefValue::get(llvm::Type::getVoidTy(context));
}

LLVMTypeDouble::LLVMTypeDouble(llvm::LLVMContext & _context)
  : LLVMType(_context)
{}
LLVMTypeDouble::~LLVMTypeDouble()
{}
llvm::Type *LLVMTypeDouble::get_type()
{
  return llvm::Type::getDoubleTy(context);
}
llvm::Value *LLVMTypeDouble::get_initializer_default()
{
  return llvm::ConstantFP::get(context, llvm::APFloat(0.0));
}

