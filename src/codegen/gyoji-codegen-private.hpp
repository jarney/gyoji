/* Copyright 2025 Jonathan S. Arney
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      https://github.com/jarney/gyoji/blob/master/LICENSE
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
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

namespace Gyoji::codegen {
    class CodeGeneratorLLVMContext {
    public:
	CodeGeneratorLLVMContext(
	    const Gyoji::context::CompilerContext & _context,
	    const Gyoji::mir::MIR & _mir
	    );
	~CodeGeneratorLLVMContext();
	void initialize();
	void generate();
	int output(const std::string & filename);
	
    private:
	Gyoji::owned<llvm::LLVMContext> TheContext;
	Gyoji::owned<llvm::IRBuilder<>> Builder;
	Gyoji::owned<llvm::Module> TheModule;
	
	const Gyoji::context::CompilerContext & compiler_context;
	const Gyoji::mir::MIR & mir;
	
	std::map<std::string, llvm::Type *> types;

	std::map<std::string, llvm::Value *> local_lvalues;
	std::map<std::string, llvm::Value *> local_variables;
	std::map<size_t, llvm::BasicBlock *> blocks;
    
	
	void create_types(const Gyoji::mir::MIR & mir);
	llvm::Type *create_type(const Gyoji::mir::Type * type);
        llvm::Type *create_type_primitive(const Gyoji::mir::Type *primitive);
        llvm::Type *create_type_enum(const Gyoji::mir::Type *enumtype);
        llvm::Type *create_type_composite(const Gyoji::mir::Type *compositetype);
        llvm::Type *create_type_pointer(const Gyoji::mir::Type *pointertype);
        llvm::Type *create_type_reference(const Gyoji::mir::Type *referencetype);
	llvm::Type *create_type_function_pointer(const Gyoji::mir::Type *fptr_type);
	llvm::Type *create_type_array(const Gyoji::mir::Type *array_type);
	llvm::Type *create_type_method_call(const Gyoji::mir::Type *method_call_type);
	

	// Global symbols
	void generate_operation_function_call(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationFunctionCall & operation
	    );

	void generate_operation_get_method(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationGetMethod & operation
	    );
	
	void generate_operation_method_get_object(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationUnary & operation
	    );

	void generate_operation_method_get_function(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationUnary & operation
	    );
	
	void generate_operation_symbol(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationSymbol & operation
	    );
	
	// Cast operations
	void generate_operation_widen_numeric(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationCast & operation
	    );

	// Indirect access
	void generate_operation_array_index(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationArrayIndex & operation
	    );
	void generate_operation_dot(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationDot & operation
	    );
	
	// Variable access
	void generate_operation_local_variable(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLocalVariable & operation
	    );
	void generate_operation_local_declare(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLocalDeclare & operation
	    );
	void generate_operation_local_undeclare(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLocalUndeclare & operation
	    );

	// Literals
	void generate_operation_literal_char(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLiteralChar & operation
	    );
	void generate_operation_literal_string(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLiteralString & operation
	    );
	void generate_operation_literal_int(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLiteralInt & operation
	    );
	void generate_operation_literal_float(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLiteralFloat & operation
	    );
	void generate_operation_literal_bool(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLiteralBool & operation
	    );
	void generate_operation_literal_null(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLiteralNull & operation
	    );

	// Unary operations
	void generate_operation_addressof(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationUnary & operation
	    );
	void generate_operation_dereference(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationUnary & operation
	    );
	void generate_operation_arithmetic_negate(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationUnary & operation
	    );
	void generate_operation_bitwise_not(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationUnary & operation
	    );
	void generate_operation_logical_not(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationUnary & operation
	    );
	
        // Binary operations
	void generate_operation_add(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_subtract(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_multiply(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_divide(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_modulo(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_logical_and(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_logical_or(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_bitwise_and(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_bitwise_or(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_bitwise_xor(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_shift(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );

        // Binary operations: comparisons
	void generate_operation_comparison(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_sizeof_type(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationSizeofType & operation
	    );
        // Binary operations: assignments
	void generate_operation_assign(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    std::map<size_t, llvm::Value *> & tmp_lvalues,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );

        // Branch and flow control
	void generate_operation_jump_conditional(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationJumpConditional & operation
	    );
	void generate_operation_jump(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationJump & operation
	    );
	llvm::Value *generate_operation_return(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationReturn & operation
	    );
	llvm::Value *generate_operation_return_void(
	    std::map<size_t, llvm::Value *> & tmp_values,
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationReturnVoid & operation
	    );
	
	llvm::Value *generate_basic_block(
	    const Gyoji::mir::Function & function,
	    const Gyoji::mir::BasicBlock & mir_block
	    );

	void generate_function(const Gyoji::mir::Function & function);
	
	llvm::Function * create_function(const Gyoji::mir::Function & function);
	llvm::AllocaInst *CreateEntryBlockAlloca(
	    llvm::Function *TheFunction,
	    const llvm::StringRef & VarName
	    );
	
    };
    
};
