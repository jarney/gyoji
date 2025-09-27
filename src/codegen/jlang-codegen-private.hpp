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
	CodeGeneratorLLVMContext(const JLang::mir::MIR & _mir);
	~CodeGeneratorLLVMContext();
	void initialize();
	void generate();
	int output(const std::string & filename);
	
    private:
	JLang::owned<llvm::LLVMContext> TheContext;
	JLang::owned<llvm::IRBuilder<>> Builder;
	JLang::owned<llvm::Module> TheModule;
	
	const JLang::mir::MIR & mir;
	
	std::map<std::string, llvm::Type *> types;
	
	std::map<std::string, llvm::Value *> NamedValues;
	
	void create_types(const JLang::mir::MIR & mir);
	llvm::Type *create_type(const JLang::mir::Type * type);
        llvm::Type *create_type_primitive(const JLang::mir::Type *primitive);
        llvm::Type *create_type_enum(const JLang::mir::Type *enumtype);
        llvm::Type *create_type_composite(const JLang::mir::Type *compositetype);
        llvm::Type *create_type_pointer(const JLang::mir::Type *pointertype);
        llvm::Type *create_type_reference(const JLang::mir::Type *referencetype);
	llvm::Type *create_type_function_pointer(const JLang::mir::Type *fptr_type);
	
	
	void generate_function(const JLang::mir::Function & function);
	
	llvm::Function * create_function(const JLang::mir::Function & function);
	llvm::AllocaInst *CreateEntryBlockAlloca(
	    llvm::Function *TheFunction,
	    const llvm::StringRef & VarName
	    );
	
    };
    
};
