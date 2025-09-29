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
	CodeGeneratorLLVMContext(
	    const JLang::context::CompilerContext & _context,
	    const JLang::mir::MIR & _mir
	    );
	~CodeGeneratorLLVMContext();
	void initialize();
	void generate();
	int output(const std::string & filename);
	
    private:
	JLang::owned<llvm::LLVMContext> TheContext;
	JLang::owned<llvm::IRBuilder<>> Builder;
	JLang::owned<llvm::Module> TheModule;
	
	const JLang::context::CompilerContext & compiler_context;
	const JLang::mir::MIR & mir;
	
	std::map<std::string, llvm::Type *> types;

	std::map<std::string, llvm::Value *> local_lvalues;
	std::map<std::string, llvm::Value *> local_variables;
	
	void create_types(const JLang::mir::MIR & mir);
	llvm::Type *create_type(const JLang::mir::Type * type);
        llvm::Type *create_type_primitive(const JLang::mir::Type *primitive);
        llvm::Type *create_type_enum(const JLang::mir::Type *enumtype);
        llvm::Type *create_type_composite(const JLang::mir::Type *compositetype);
        llvm::Type *create_type_pointer(const JLang::mir::Type *pointertype);
        llvm::Type *create_type_reference(const JLang::mir::Type *referencetype);
	llvm::Type *create_type_function_pointer(const JLang::mir::Type *fptr_type);
	

	void generate_operation_function_call(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationFunctionCall *operation
	    );
	void generate_operation_symbol(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationSymbol *operation
	    );
	void generate_operation_local_variable(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationLocalVariable *operation
	    );
	
	void generate_operation_local_declare(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationLocalDeclare *operation
	    );
	void generate_operation_local_undeclare(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationLocalUndeclare *operation
	    );
	void generate_operation_literal_char(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationLiteralChar *operation
	    );
	void generate_operation_literal_string(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationLiteralString *operation
	    );
	void generate_operation_literal_int(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationLiteralInt *operation
	    );
	void generate_operation_literal_float(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationLiteralFloat *operation
	    );
	void generate_operation_post_increment(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationUnary *operation
	    );
	void generate_operation_post_decrement(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationUnary *operation
	    );
	void generate_operation_pre_increment(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationUnary *operation
	    );
	void generate_operation_pre_decrement(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationUnary *operation
	    );
	void generate_operation_widen_numeric(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationCast *operation
	    );
	void generate_operation_add(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary *operation
	    );
	void generate_operation_subtract(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary *operation
	    );
	void generate_operation_multiply(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary *operation
	    );
	void generate_operation_divide(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary *operation
	    );
	void generate_operation_modulo(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary *operation
	    );
	void generate_operation_assign(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary *operation
	    );
	void generate_operation_jump_if_equal(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationJumpIfEqual *operation
	    );
	void generate_operation_jump(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationJump *operation
	    );
	llvm::Value *generate_operation_return(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationReturn *operation
	    );
	
	llvm::Value *generate_basic_block(const JLang::mir::Function & function, size_t blockid);
	void generate_function(const JLang::mir::Function & function);
	
	llvm::Function * create_function(const JLang::mir::Function & function);
	llvm::AllocaInst *CreateEntryBlockAlloca(
	    llvm::Function *TheFunction,
	    const llvm::StringRef & VarName
	    );
	
    };
    
};
