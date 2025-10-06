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
	std::map<size_t, llvm::BasicBlock *> blocks;
    
	
	void create_types(const JLang::mir::MIR & mir);
	llvm::Type *create_type(const JLang::mir::Type * type);
        llvm::Type *create_type_primitive(const JLang::mir::Type *primitive);
        llvm::Type *create_type_enum(const JLang::mir::Type *enumtype);
        llvm::Type *create_type_composite(const JLang::mir::Type *compositetype);
        llvm::Type *create_type_pointer(const JLang::mir::Type *pointertype);
        llvm::Type *create_type_reference(const JLang::mir::Type *referencetype);
	llvm::Type *create_type_function_pointer(const JLang::mir::Type *fptr_type);
	llvm::Type *create_type_array(const JLang::mir::Type *array_type);
	

	// Global symbols
	void generate_operation_function_call(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationFunctionCall & operation
	    );
	void generate_operation_symbol(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationSymbol & operation
	    );
	
	// Cast operations
	void generate_operation_widen_numeric(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationCast & operation
	    );

	// Indirect access
	void generate_operation_array_index(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationArrayIndex & operation
	    );
	void generate_operation_dot(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationDot & operation
	    );
	
	// Variable access
	void generate_operation_local_variable(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationLocalVariable & operation
	    );
	void generate_operation_local_declare(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationLocalDeclare & operation
	    );
	void generate_operation_local_undeclare(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationLocalUndeclare & operation
	    );

	// Literals
	void generate_operation_literal_char(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationLiteralChar & operation
	    );
	void generate_operation_literal_string(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationLiteralString & operation
	    );
	void generate_operation_literal_int(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationLiteralInt & operation
	    );
	void generate_operation_literal_float(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationLiteralFloat & operation
	    );
	void generate_operation_literal_bool(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationLiteralBool & operation
	    );
	void generate_operation_literal_null(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationLiteralNull & operation
	    );

	// Unary operations
	void generate_operation_addressof(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationUnary & operation
	    );
	void generate_operation_dereference(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationUnary & operation
	    );
	void generate_operation_arithmetic_negate(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationUnary & operation
	    );
	void generate_operation_bitwise_not(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationUnary & operation
	    );
	void generate_operation_logical_not(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationUnary & operation
	    );
	
        // Binary operations
	void generate_operation_add(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary & operation
	    );
	void generate_operation_subtract(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary & operation
	    );
	void generate_operation_multiply(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary & operation
	    );
	void generate_operation_divide(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary & operation
	    );
	void generate_operation_modulo(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary & operation
	    );
	void generate_operation_logical_and(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary & operation
	    );
	void generate_operation_logical_or(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary & operation
	    );
	void generate_operation_bitwise_and(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary & operation
	    );
	void generate_operation_bitwise_or(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary & operation
	    );
	void generate_operation_bitwise_xor(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary & operation
	    );
	void generate_operation_shift(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary & operation
	    );

        // Binary operations: comparisons
	void generate_operation_comparison(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary & operation
	    );
	void generate_operation_sizeof_type(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationSizeofType & operation
	    );
        // Binary operations: assignments
	void generate_operation_assign(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationBinary & operation
	    );

        // Branch and flow control
	void generate_operation_jump_conditional(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationJumpConditional & operation
	    );
	void generate_operation_jump(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationJump & operation
	    );
	llvm::Value *generate_operation_return(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const JLang::mir::Function & mir_function,
	    const JLang::mir::OperationReturn & operation
	    );
	
	llvm::Value *generate_basic_block(
	    const JLang::mir::Function & function,
	    const JLang::mir::BasicBlock & mir_block
	    );

	void generate_function(const JLang::mir::Function & function);
	
	llvm::Function * create_function(const JLang::mir::Function & function);
	llvm::AllocaInst *CreateEntryBlockAlloca(
	    llvm::Function *TheFunction,
	    const llvm::StringRef & VarName
	    );
	
    };
    
};
