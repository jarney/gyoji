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
	std::map<size_t, llvm::Value *> tmp_values;
	std::map<size_t, llvm::Value *> tmp_lvalues;
	
	void create_types(const Gyoji::mir::MIR & mir);
	llvm::Type *create_type(const Gyoji::mir::Type * type);
        llvm::Type *create_type_primitive(const Gyoji::mir::Type *primitive);
        llvm::Type *create_type_enum(const Gyoji::mir::Type *enumtype);
        llvm::Type *create_type_composite(const Gyoji::mir::Type *compositetype);
        llvm::Type *create_type_pointer(const Gyoji::mir::Type *pointertype);
        llvm::Type *create_type_reference(const Gyoji::mir::Type *referencetype);
	llvm::Type *create_type_function(const Gyoji::mir::Type *function_type);
	llvm::Type *create_type_function_pointer(const Gyoji::mir::Type *fptr_type);
	llvm::Type *create_type_array(const Gyoji::mir::Type *array_type);

	// Global symbols
	void generate_operation_function_call(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationFunctionCall & operation
	    );

	void generate_operation_symbol(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationSymbol & operation
	    );
	
	// Cast operations
	void generate_operation_widen_numeric(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationCast & operation
	    );

	// Indirect access
	void generate_operation_array_index(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationArrayIndex & operation
	    );
	void generate_operation_dot(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationDot & operation
	    );
	
	// Variable access
	void generate_operation_local_variable(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLocalVariable & operation
	    );
	void generate_operation_local_declare(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLocalDeclare & operation
	    );
	void generate_operation_local_undeclare(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLocalUndeclare & operation
	    );

	// Literals
	void generate_operation_literal_char(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLiteralChar & operation
	    );
	void generate_operation_literal_string(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLiteralString & operation
	    );
	void generate_operation_literal_int(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLiteralInt & operation
	    );
	void generate_operation_literal_float(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLiteralFloat & operation
	    );
	void generate_operation_literal_bool(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLiteralBool & operation
	    );
	void generate_operation_literal_null(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationLiteralNull & operation
	    );	
	void generate_operation_anonymous_structure(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationAnonymousStructure &operation
	    );

	// Unary operations
	void generate_operation_addressof(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationUnary & operation
	    );
	void generate_operation_dereference(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationUnary & operation
	    );
	void generate_operation_arithmetic_negate(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationUnary & operation
	    );
	void generate_operation_bitwise_not(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationUnary & operation
	    );
	void generate_operation_logical_not(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationUnary & operation
	    );
	
        // Binary operations
	void generate_operation_add(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_subtract(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_multiply(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_divide(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_modulo(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_logical_and(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_logical_or(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_bitwise_and(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_bitwise_or(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_bitwise_xor(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_shift(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );

        // Binary operations: comparisons
	void generate_operation_comparison(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );
	void generate_operation_sizeof_type(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationSizeofType & operation
	    );
        // Binary operations: assignments
	void generate_operation_assign(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationBinary & operation
	    );

        // Branch and flow control
	void generate_operation_jump_conditional(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationJumpConditional & operation
	    );
	void generate_operation_jump(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationJump & operation
	    );
	llvm::Value *generate_operation_return(
	    const Gyoji::mir::Function & mir_function,
	    const Gyoji::mir::OperationReturn & operation
	    );
	llvm::Value *generate_operation_return_void(
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
