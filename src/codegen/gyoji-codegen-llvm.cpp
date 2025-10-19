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
#include <gyoji-codegen.hpp>
#include "gyoji-codegen-private.hpp"
#include <gyoji-misc/jstring.hpp>

using namespace llvm::sys;
using namespace Gyoji::codegen;
using namespace Gyoji::mir;

CodeGeneratorLLVM::CodeGeneratorLLVM(
    const Gyoji::context::CompilerContext & _compiler_context,
    const Gyoji::mir::MIR & _mir
    )
    : context(std::make_unique<CodeGeneratorLLVMContext>(_compiler_context, _mir))
{}

CodeGeneratorLLVM::~CodeGeneratorLLVM()
{}

void
CodeGeneratorLLVM::initialize()
{ context->initialize(); }

void
CodeGeneratorLLVM::generate()
{ context->generate(); }

int
CodeGeneratorLLVM::output(const std::string & filename)
{ return context->output(filename); }
/////////////////////////////////////
// CodeGeneratorLLVMContext
/////////////////////////////////////
CodeGeneratorLLVMContext::CodeGeneratorLLVMContext(
    const Gyoji::context::CompilerContext & _compiler_context,
    const Gyoji::mir::MIR & _mir
    )
    : compiler_context(_compiler_context)
    , mir(_mir)
{}
CodeGeneratorLLVMContext::~CodeGeneratorLLVMContext()
{}

void
CodeGeneratorLLVMContext::initialize()
{
    // Open a new context and module.
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("gyoji LLVM Code Generator", *TheContext);
    // Create a new builder for the module.
    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
    
    //  register_type_builtins();
    //  register_operator_builtins();
}

llvm::Function *
CodeGeneratorLLVMContext::create_function(const Function & function)
{
    // Make the function type:  double(double,double) etc.
    std::vector<llvm::Type *> llvm_arguments;
    const std::vector<FunctionArgument> & function_arguments = function.get_arguments();
    
    for (const auto & semantic_arg : function_arguments) {
	llvm::Type *atype = types[semantic_arg.get_type()->get_name()];
	llvm_arguments.push_back(atype);
    }
    
    llvm::Type* return_value_type = types[function.get_return_type()->get_name()];

    llvm::FunctionType *FT =
	llvm::FunctionType::get(return_value_type, llvm_arguments, false);
    
    std::string method_name = function.get_name();

    llvm::Function *F =
	llvm::Function::Create(FT, llvm::Function::ExternalLinkage, method_name, TheModule.get());
    
    // Set names for all arguments.
    unsigned Idx = 0;
    for (auto &Arg : F->args()) {
	const auto & semantic_arg = function_arguments.at(Idx++);
	Arg.setName(semantic_arg.get_name());
    }
    
    return F;
}

/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
/// the function.  This is used for mutable variables etc.
llvm::AllocaInst *CodeGeneratorLLVMContext::CreateEntryBlockAlloca(
    llvm::Function *TheFunction,
    const llvm::StringRef & VarName
    )
{
    llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
			   TheFunction->getEntryBlock().begin()
	);
    return TmpB.CreateAlloca(llvm::Type::getDoubleTy(*TheContext), nullptr, VarName);
}

/**
 * An enum type is "almost" a primitive type.
 * It always resolves to a u32 on the physical host
 * but carries some slightly different semantics.
 */
llvm::Type *
CodeGeneratorLLVMContext::create_type_enum(const Gyoji::mir::Type *enumtype)
{
    return llvm::Type::getInt32Ty(*TheContext);
}

llvm::Type *
CodeGeneratorLLVMContext::create_type_composite(const Gyoji::mir::Type *compositetype)
{
    std::vector<llvm::Type*> members;

    // Note that this relies on the fact that
    // the members are in order and equal to the
    // 'index' of the member.  Yes, this is a bit
    // of a hacky way to do that.
    for (const auto & member : compositetype->get_members()) {
	members.push_back(create_type(member.get_type()));
    }
    
    llvm::Type *llvm_type = llvm::StructType::create(*TheContext, members, compositetype->get_name());
    types.insert(std::pair<std::string, llvm::Type*>(
		     compositetype->get_name(),
		     llvm_type
		     )
	);
    return llvm_type;
}

llvm::Type *
CodeGeneratorLLVMContext::create_type_pointer(const Gyoji::mir::Type *pointertype)
{
    const Gyoji::mir::Type * pointer_target = pointertype->get_pointer_target();
    llvm::Type * llvm_type =
	llvm::PointerType::get(create_type(pointer_target),
			       0 // Address space (default to 0?  This seems unclean, llvm!)
	    );
    types.insert(std::pair<std::string, llvm::Type*>(
		     pointertype->get_name(),
		     llvm_type
		     )
	);
    return llvm_type;
}

llvm::Type *
CodeGeneratorLLVMContext::create_type_reference(const Gyoji::mir::Type *referencetype)
{
    const Gyoji::mir::Type * pointer_target = referencetype->get_pointer_target();
    llvm::Type * llvm_type =
	llvm::PointerType::get(create_type(pointer_target),
			       0 // Address space (default to 0?  This seems unclean, llvm!)
	    );
    types.insert(std::pair<std::string, llvm::Type*>(
		     referencetype->get_name(),
		     llvm_type
		     )
	);
    return llvm_type;
}

llvm::Type *
CodeGeneratorLLVMContext::create_type_array(const Gyoji::mir::Type *array_type)
{
    const Gyoji::mir::Type * element_type = array_type->get_pointer_target();
    llvm::Type * llvm_element_type =
	llvm::PointerType::get(create_type(element_type),
			       0 // Address space (default to 0?  This seems unclean, llvm!)
	    );
    llvm::Type *llvm_array_type = llvm::ArrayType::get(llvm_element_type, array_type->get_array_length());

    types.insert(std::pair<std::string, llvm::Type*>(
		     array_type->get_name(),
		     llvm_array_type
		     )
	);
    return llvm_array_type;
}

llvm::Type *
CodeGeneratorLLVMContext::create_type_function_pointer(const Gyoji::mir::Type *fptr_type)
{
    const Gyoji::mir::Type *mir_return_type = fptr_type->get_return_type();
    llvm::Type *llvm_return_type = create_type(mir_return_type);
    
    const std::vector<Gyoji::mir::Argument> & mir_args = fptr_type->get_argument_types();
    
    std::vector<llvm::Type *> llvm_fptr_args;
    for (const auto & mir_arg : mir_args) {
	llvm_fptr_args.push_back(create_type(mir_arg.get_type()));
    }
    
    llvm::ArrayRef<llvm::Type *> llvm_args_ref(llvm_fptr_args);
    llvm::FunctionType *llvm_fptr_type = llvm::FunctionType::get(llvm_return_type, llvm_args_ref, false);
    
    types.insert(std::pair<std::string, llvm::Type*>(
		     fptr_type->get_name(),
		     llvm_fptr_type
		     )
	);
    return llvm_fptr_type;
}

llvm::Type *
CodeGeneratorLLVMContext::create_type_method_call(const Gyoji::mir::Type *method_call_type)
{
    std::vector<llvm::Type*> members;

    // Note that this relies on the fact that
    // the members are in order and equal to the
    // 'index' of the member.  Yes, this is a bit
    // of a hacky way to do that.

    const Gyoji::mir::Type *u32_type = mir.get_types().get_type("u32");
    members.push_back(create_type(u32_type));
    members.push_back(create_type(method_call_type->get_function_pointer_type()));
    
    llvm::Type *llvm_method_call_type = llvm::StructType::create(*TheContext, members, method_call_type->get_name());

    types.insert(std::pair<std::string, llvm::Type*>(
		     method_call_type->get_name(),
		     llvm_method_call_type
		     )
	);
    return llvm_method_call_type;
}



/**
 * These are the truly primitive types.
 * They are defined in the language
 * and have their basis in physical computing
 * units (number of bits).
 */
llvm::Type *
CodeGeneratorLLVMContext::create_type_primitive(const Type *primitive)
{
    llvm::Type *llvm_type;

    switch (primitive->get_type()) {
    // Unsigned integer types
    case Type::TYPE_PRIMITIVE_u8:
	llvm_type = llvm::Type::getInt8Ty(*TheContext);
	break;
    case Type::TYPE_PRIMITIVE_u16:
	llvm_type = llvm::Type::getInt16Ty(*TheContext);
	break;
    case Type::TYPE_PRIMITIVE_u32:
	llvm_type = llvm::Type::getInt32Ty(*TheContext);
	break;
    case Type::TYPE_PRIMITIVE_u64:
	llvm_type = llvm::Type::getInt64Ty(*TheContext);
	break;

    // Signed integer types
    case Type::TYPE_PRIMITIVE_i8:
	llvm_type = llvm::Type::getInt8Ty(*TheContext);
	break;
    case Type::TYPE_PRIMITIVE_i16:
	llvm_type = llvm::Type::getInt16Ty(*TheContext);
	break;
    case Type::TYPE_PRIMITIVE_i32:
	llvm_type = llvm::Type::getInt32Ty(*TheContext);
	break;
    case Type::TYPE_PRIMITIVE_i64:
	llvm_type = llvm::Type::getInt64Ty(*TheContext);
	break;
    case Type::TYPE_PRIMITIVE_f32:
	llvm_type = llvm::Type::getFloatTy(*TheContext);
	break;
    case Type::TYPE_PRIMITIVE_f64:
	llvm_type = llvm::Type::getDoubleTy(*TheContext);
	break;
    // "Special" types
    case Type::TYPE_PRIMITIVE_bool:
	llvm_type = llvm::Type::getInt32Ty(*TheContext);
	break;
    case Type::TYPE_PRIMITIVE_void:
	llvm_type = llvm::Type::getVoidTy(*TheContext);
	break;
    default:
	fprintf(stderr, "Compiler BUG!  Unknown primitive type passed to code generator\n");
	exit(1);
    }
    
    types.insert(std::pair<std::string, llvm::Type*>(
		     primitive->get_name(),
		     llvm_type
		     )
	);
    return llvm_type;
}

llvm::Type *
CodeGeneratorLLVMContext::create_type(const Type * type)
{
    // First, check if we have (recursively)
    // defined this type already.  Skip it if we have.
    // This is not a duplicate definition because the
    // type resolver will already have checked for that
    // case and reject any truly duplicate types.
    const auto it = types.find(type->get_name());
    if (it != types.end()) {
	return it->second;
    }
    
    if (type->is_primitive()) {
	return create_type_primitive(type);
    }
    else if (type->is_enum()) {
	return create_type_enum(type);
    }
    else if (type->is_composite()) {
	return create_type_composite(type);
    }
    else if (type->is_pointer()) {
	return create_type_pointer(type);
    }
    else if (type->is_reference()) {
	return create_type_reference(type);
    }
    else if (type->is_function_pointer()) {
	return create_type_function_pointer(type);
    }
    else if (type->is_array()) {
	return create_type_array(type);
    }
    else if (type->is_method_call()) {
	return create_type_method_call(type);
    }
    fprintf(stderr, "Compiler BUG!  Unknown type type passed to code generator %s\n", type->get_name().c_str());
    exit(1);
    return nullptr;
}

void
CodeGeneratorLLVMContext::create_types(const MIR & _mir)
{
    for (const auto & type_el : _mir.get_types().get_types()) {
	const Type * type = type_el.second.get();
	
	// We can safely skip generating incomplete
	// types without raising an error because
	// the analysis stage will alert us if there
	// is an incomplete type that is used
	// in another type or in a method/function.
	if (!type->is_complete()) {
	    continue;
	}
	create_type(type);
    }
}


void
CodeGeneratorLLVMContext::generate()
{
    create_types(mir);
    
    const Functions & functions = mir.get_functions();
    for (auto const & function : functions.get_functions()) {
	fprintf(stderr, " - Generating function %s\n", function->get_name().c_str());
	generate_function(*function);
    }
}

// Global symbols
void
CodeGeneratorLLVMContext::generate_operation_function_call(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationFunctionCall & operation
    )
{

    // First argument of this operand is the function to call.
    size_t function_operand = operation.get_operands().at(0);
    llvm::Value *llvm_function = tmp_values[function_operand];

    // The remaining operands are the values to pass to it.
    std::vector<llvm::Value *> llvm_args;
    const std::vector<size_t> & operands = operation.get_operands();;
    for (size_t i = 0; i < operands.size()-1; i++) {
	llvm::Value *llvm_arg = tmp_values[operands.at(i+1)];
	llvm_args.push_back(llvm_arg);
    }
    const Type *mir_type = mir_function.tmpvar_get(function_operand);
    llvm::Type *llvm_fptr_type = types[mir_type->get_name()];

    Builder->CreateCall((llvm::FunctionType*)llvm_fptr_type, (llvm::Function*)llvm_function, llvm_args);

}

void
CodeGeneratorLLVMContext::generate_operation_get_method(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationGetMethod & operation
    )
{
    const Gyoji::mir::Symbol *symbol = mir.get_symbols().get_symbol(operation.get_method());
    llvm::Type *fptr_type = types[symbol->get_type()->get_name()];

    // This isn't a 'real' value,
    // it's intended as a way to pass the
    // value from here into the 'method_get_object'
    // without putting it on the heap or anything.
    size_t object_tmpvar = operation.get_operands().at(0);
    llvm::Constant * object_value = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*TheContext), object_tmpvar);

    std::string method_name = operation.get_method();
    
    llvm::Function *fptr_value = TheModule->getFunction(method_name);
    if (fptr_value == nullptr) {
	fptr_value = llvm::Function::Create((llvm::FunctionType*)fptr_type, llvm::Function::ExternalLinkage, method_name, TheModule.get());
	if (fptr_value == nullptr) {
	    fprintf(stderr, "Could not find function for symbol %s\n", operation.get_method().c_str());
	    exit(1);
	}
    }
    
    // What we actually push here is the
    // tmpvar id of the object and not the object itself.
    // This is because we don't actually know it yet,
    // that will be resolved later in the 'method_get_object'
    // method.

    const Gyoji::mir::Type *mir_struct_type = mir_function.tmpvar_get(operation.get_result());
    llvm::Type *struct_type = types[mir_struct_type->get_name()];
    
    std::vector<llvm::Constant*> struct_values;
    struct_values.push_back(object_value);
    struct_values.push_back(fptr_value);
    llvm::Value *result = llvm::ConstantStruct::get((llvm::StructType*)struct_type, struct_values);
    
    tmp_values.insert(std::pair(operation.get_result(), result));
}
void
CodeGeneratorLLVMContext::generate_operation_method_get_object(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationUnary & operation
    )
{
    // Get the struct and pull out the "object's"
    // tmpvar.
    llvm::Value *method_call_value = tmp_values[operation.get_a()];
    llvm::ConstantStruct *method_call_constant_struct = (llvm::ConstantStruct*)method_call_value;
    llvm::Constant *object_tmpvar_el = method_call_constant_struct->getAggregateElement((unsigned)0);
    llvm::ConstantInt * object_tmpvar_int = (llvm::ConstantInt*)object_tmpvar_el;
    size_t object_tmpvar = object_tmpvar_int->getValue().getZExtValue();


    // This is where we indirect the object to get its pointer
    // We take the 'lvalue' (pointer) and put it into the value
    // The returned object is no longer an lvalue.
    llvm::Value *object_value = tmp_lvalues[object_tmpvar];
    tmp_values.insert(std::pair(operation.get_result(), object_value));
}
void
CodeGeneratorLLVMContext::generate_operation_method_get_function(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationUnary & operation
    )
{
    // Get the struct and pull out the "function pointer"
    // tmpvar.
    llvm::Value *method_call_value = tmp_values[operation.get_a()];
    llvm::ConstantStruct *method_call_constant_struct = (llvm::ConstantStruct*)method_call_value;
    llvm::Constant *fptr_value = method_call_constant_struct->getAggregateElement((unsigned)1);
    tmp_values.insert(std::pair(operation.get_result(), fptr_value));
}

	

void
CodeGeneratorLLVMContext::generate_operation_symbol(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationSymbol & operation
    )
{
    std::string symbol_name = operation.get_symbol_name();
    const Gyoji::mir::Symbol *symbol = mir.get_symbols().get_symbol(symbol_name);
    
    llvm::Type *fptr_type = types[symbol->get_type()->get_name()];
    if (fptr_type == nullptr) {
	fprintf(stderr, "Could not find function pointer type for %s\n", symbol->get_type()->get_name().c_str());
	exit(1);
    }

    // TODO : We handle functions here,
    // but we should also handle global variables
    // when we get to it, even though globals
    // and statics are evil incarnate.

    llvm::Function *F = TheModule->getFunction(symbol_name);
    if (F == nullptr) {
	F = llvm::Function::Create((llvm::FunctionType*)fptr_type, llvm::Function::ExternalLinkage, symbol_name, TheModule.get());
	if (F == nullptr) {
	    fprintf(stderr, "Could not find function for symbol %s\n", symbol_name.c_str());
	    exit(1);
	}
    }

    tmp_values.insert(std::pair(operation.get_result(), F));
}

// Cast operations
void
CodeGeneratorLLVMContext::generate_operation_widen_numeric(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationCast & operation
    )
{
    size_t a = operation.get_a();
    const Gyoji::mir::Type *atype = mir_function.tmpvar_get(a);
    if (!atype->is_numeric()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for add operator.",
		std::string("Invalid operands for add operation.  Operand must be a numeric type, but were ") + atype->get_name()
		);
	return;
    }
    llvm::Value *value_a = tmp_values[a];
    llvm::Type *llvm_cast_type = types[operation.get_cast_type()->get_name()];
    if (atype->is_integer()) {
	llvm::Value *sum = Builder->CreateIntCast(value_a, llvm_cast_type, atype->is_signed());
	tmp_values.insert(std::pair(operation.get_result(), sum));
    }
    else {
	llvm::Value *sum = Builder->CreateFPCast(value_a, llvm_cast_type);
	tmp_values.insert(std::pair(operation.get_result(), sum));
    }
}

// Indirect access
void
CodeGeneratorLLVMContext::generate_operation_array_index(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationArrayIndex & operation
    )
{
    size_t array_tmpvar = operation.get_a();
    size_t index_tmpvar = operation.get_b();
    
    const Gyoji::mir::Type *mir_array_type = mir_function.tmpvar_get(array_tmpvar);
    const Gyoji::mir::Type *mir_array_element_type = mir_array_type->get_pointer_target();
    llvm::Type *llvm_array_type = types[mir_array_type->get_name()];
    llvm::Type *llvm_array_element_type = types[mir_array_element_type->get_name()];

    llvm::Value *array_lvalue = tmp_lvalues[array_tmpvar];
    llvm::Value *index_value = tmp_values[index_tmpvar];

    std::vector<llvm::Value *> indices;
    indices.push_back(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*TheContext), 0L));
    indices.push_back(index_value);
    llvm::Value *addressofelement = Builder->CreateInBoundsGEP(llvm_array_type, array_lvalue, indices);
    llvm::Value *value = Builder->CreateLoad(llvm_array_element_type, addressofelement);
    
    tmp_lvalues.insert(std::pair(operation.get_result(), addressofelement));
    tmp_values.insert(std::pair(operation.get_result(), value));
}
void
CodeGeneratorLLVMContext::generate_operation_dot(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationDot & operation
    )
{
    size_t a = operation.get_a();
    // The type is a class
    const Gyoji::mir::Type *mir_class_type = mir_function.tmpvar_get(a);
    llvm::Type *llvm_class_type = types[mir_class_type->get_name()];

    const std::string & member_name = operation.get_member_name();
    const TypeMember *member = mir_class_type->member_get(member_name);
    size_t member_index = member->get_index();
    
    llvm::Value *value_a = tmp_lvalues[a];
    llvm::Value *result = Builder->CreateConstInBoundsGEP2_32(llvm_class_type, value_a, 0, member_index);
    llvm::Value *value = Builder->CreateLoad(types[member->get_type()->get_name()], result);
    
    tmp_lvalues.insert(std::pair(operation.get_result(), result));
    tmp_values.insert(std::pair(operation.get_result(), value));
}

// Variable access
void
CodeGeneratorLLVMContext::generate_operation_local_variable(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationLocalVariable & operation
    )
{
    llvm::Type *type = types[operation.get_var_type()->get_name()];
    llvm::Value *variable_ptr = local_variables[operation.get_symbol_name()];
    llvm::Value *value = Builder->CreateLoad(type, variable_ptr);
    tmp_lvalues.insert(std::pair(operation.get_result(), variable_ptr));
    tmp_values.insert(std::pair(operation.get_result(), value));
}
void
CodeGeneratorLLVMContext::generate_operation_local_declare(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationLocalDeclare & operation
    )
{
    llvm::Type *type = types[operation.get_variable_type()->get_name()];
    llvm::Value *value = Builder->CreateAlloca(type, nullptr, operation.get_variable());
    local_variables[operation.get_variable()] = value;
}
void
CodeGeneratorLLVMContext::generate_operation_local_undeclare(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationLocalUndeclare & operation
    )
{ /* There's nothing to implement here, this is just an MIR thing. */ }

// Literals
void
CodeGeneratorLLVMContext::generate_operation_literal_char(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationLiteralChar & operation
    )
{
    char c = operation.get_literal_char();
    llvm::Value * result = llvm::ConstantInt::get(llvm::Type::getInt8Ty(*TheContext), c);
    tmp_values.insert(std::pair(operation.get_result(), result));
}
void
CodeGeneratorLLVMContext::generate_operation_literal_string(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationLiteralString & operation
    )
{
    llvm::Constant *string_constant = llvm::ConstantDataArray::getString(*TheContext, operation.get_literal_string());
    llvm::GlobalVariable* v = new llvm::GlobalVariable(
	*TheModule,
	string_constant->getType(),
	true,
	llvm::GlobalValue::InternalLinkage,
	string_constant
	);
    
    tmp_values.insert(std::pair(operation.get_result(), v));
}
void
CodeGeneratorLLVMContext::generate_operation_literal_int(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationLiteralInt & operation
    )
{
    llvm::Value *value;
    switch (operation.get_literal_type()) {
    case Type::TYPE_PRIMITIVE_u8:
        {
	    value = Builder->getInt8(operation.get_literal_u8());
	}
	break;
    case Type::TYPE_PRIMITIVE_u16:
        {
	    value = Builder->getInt16(operation.get_literal_u16());
	}
	break;
    case Type::TYPE_PRIMITIVE_u32:
        {
	    value = Builder->getInt32(operation.get_literal_u32());
	}
	break;
    case Type::TYPE_PRIMITIVE_u64:
        {
	    value = Builder->getInt64(operation.get_literal_u64());
	}
	break;
    case Type::TYPE_PRIMITIVE_i8:
        {
	    value = Builder->getInt8(operation.get_literal_i8());
	}
	break;
    case Type::TYPE_PRIMITIVE_i16:
        {
	    value = Builder->getInt16(operation.get_literal_i16());
	}
	break;
    case Type::TYPE_PRIMITIVE_i32:
        {
	    value = Builder->getInt32(operation.get_literal_i32());
	}
	break;
    case Type::TYPE_PRIMITIVE_i64:
        {
	    value = Builder->getInt64(operation.get_literal_i64());
	}
	break;
    default:
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for literal int.",
		std::string("Literal int has unknown type") + std::to_string(operation.get_literal_type())
		);
	return;
    }
    tmp_values.insert(std::pair(operation.get_result(), value));
}
void
CodeGeneratorLLVMContext::generate_operation_literal_float(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationLiteralFloat & operation
    )
{
    switch (operation.get_literal_type()) {
    case Type::TYPE_PRIMITIVE_f32:
    {
	llvm::Type* llvm_type = llvm::Type::getFloatTy(*TheContext);
	llvm::Value * result = llvm::ConstantFP::get(llvm_type, operation.get_literal_float());
	tmp_values.insert(std::pair(operation.get_result(), result));
    }
	break;
    case Type::TYPE_PRIMITIVE_f64:
    {
	llvm::Type *llvm_type = llvm::Type::getDoubleTy(*TheContext);
	llvm::Value * result = llvm::ConstantFP::get(llvm_type, operation.get_literal_double());
	tmp_values.insert(std::pair(operation.get_result(), result));
    }
	break;
    default:
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for literal float.",
		std::string("Literal float has unknown type") + std::to_string(operation.get_literal_type())
		);
	return;
    }
}

void
CodeGeneratorLLVMContext::generate_operation_literal_bool(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationLiteralBool & operation
    )
{
    llvm::Value * result = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*TheContext), operation.get_literal_bool() ? 1 : 0);
    tmp_values.insert(std::pair(operation.get_result(), result));
}
void
CodeGeneratorLLVMContext::generate_operation_literal_null(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationLiteralNull & operation
    )
{
    llvm::Type * llvm_void_type = create_type(mir.get_types().get_type("u8"));
    llvm::PointerType * llvm_voidstar_type =
	llvm::PointerType::get(llvm_void_type,
			       0 // Address space (default to 0?  This seems unclean, llvm!)
	    );
    llvm::Value *result = llvm::ConstantPointerNull::get(llvm_voidstar_type);
    tmp_values.insert(std::pair(operation.get_result(), result));
}


// Unary operations
void
CodeGeneratorLLVMContext::generate_operation_addressof(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationUnary & operation
    )
{
    // The type is a class
    const auto & found = tmp_lvalues.find(operation.get_a());
    if (found == tmp_lvalues.end()) {
	fprintf(stderr, "This is not an lvalue\n");
	exit(2);
    }
    // Addressof returns a value (a pointer value)
    // but itself is not an lvalue because it cannot
    // be assigned to.
    tmp_values.insert(std::pair(operation.get_result(), found->second));
}

void
CodeGeneratorLLVMContext::generate_operation_dereference(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationUnary & operation
    )
{

    size_t a = operation.get_a();
    // The type is a class
    const Gyoji::mir::Type *mir_pointer_type = mir_function.tmpvar_get(a);
    if (!mir_pointer_type->is_pointer() && !mir_pointer_type->is_reference()) {
	fprintf(stderr, "Address of thing being referenced is not a pointer\n");
	exit(1);
    }
    const Gyoji::mir::Type *mir_pointer_target = mir_pointer_type->get_pointer_target();
    llvm::Type *llvm_pointer_target = types[mir_pointer_target->get_name()];

    llvm::Value *value_a = tmp_values[a];
    llvm::Value *result = Builder->CreateLoad(llvm_pointer_target, value_a);

    tmp_lvalues.insert(std::pair(operation.get_result(), value_a));
    tmp_values.insert(std::pair(operation.get_result(), result));
}

void
CodeGeneratorLLVMContext::generate_operation_arithmetic_negate(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationUnary & operation
    )
{
    size_t a = operation.get_a();
    const Gyoji::mir::Type *atype = mir_function.tmpvar_get(a);
    if (!atype->is_signed()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand arithmetic negate",
		std::string("Operand to arithmetic negate must be a signed integer but was ") + atype->get_name()
		);
	return;
    }
    
    llvm::Value * avalue = tmp_values[a];
    llvm::Value * result = Builder->CreateNeg(avalue);
    tmp_values.insert(std::pair(operation.get_result(), result));
}
void
CodeGeneratorLLVMContext::generate_operation_bitwise_not(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationUnary & operation
    )
{
    size_t a = operation.get_a();
    const Gyoji::mir::Type *atype = mir_function.tmpvar_get(a);
    if (!atype->is_unsigned()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand bitwise not",
		std::string("Operand to bitwise not must be an unsigned integer but was ") + atype->get_name()
		);
	return;
    }

    llvm::Value * avalue = tmp_values[a];
    llvm::Value * result = Builder->CreateNot(avalue);
    tmp_values.insert(std::pair(operation.get_result(), result));
}

void
CodeGeneratorLLVMContext::generate_operation_logical_not(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationUnary & operation
    )
{
    size_t a = operation.get_a();
    const Gyoji::mir::Type *atype = mir_function.tmpvar_get(a);
    if (!atype->is_bool()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand logical not",
		std::string("Operand to logical not must be a bool but was ") + atype->get_name()
		);
	return;
    }
    
    llvm::Value * avalue = tmp_values[a];
    llvm::Value * result = Builder->CreateNot(avalue);
    tmp_values.insert(std::pair(operation.get_result(), result));
}

void
CodeGeneratorLLVMContext::generate_operation_sizeof_type(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationSizeofType & operation
    )
{
    llvm::Type *llvm_type = types[operation.get_type()->get_name()];
    llvm::Value * result = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*TheContext), TheModule->getDataLayout().getTypeAllocSize(llvm_type));
    tmp_values.insert(std::pair(operation.get_result(), result));
}


// Binary operations: arithmetic
void
CodeGeneratorLLVMContext::generate_operation_add(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationBinary & operation
    )
{
    size_t a = operation.get_a();
    size_t b = operation.get_b();
    const Gyoji::mir::Type *atype = mir_function.tmpvar_get(a);
    const Gyoji::mir::Type *btype = mir_function.tmpvar_get(b);
    if (!atype->is_numeric() || !btype->is_numeric()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for add operator.",
		std::string("Invalid operands for add operation.  Operand must be a numeric type, but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
    llvm::Value *value_a = tmp_values[a];
    llvm::Value *value_b = tmp_values[b];

    if (atype->is_integer() && btype->is_integer()) {
	llvm::Value *sum = Builder->CreateAdd(value_a, value_b);
	tmp_values.insert(std::pair(operation.get_result(), sum));
    }
    else if (atype->is_float() && btype->is_float()) {
	llvm::Value *sum = Builder->CreateFAdd(value_a, value_b);
	tmp_values.insert(std::pair(operation.get_result(), sum));
    }
    else {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for add operator.",
		std::string("Operands must be both integer or floating-point primitive types but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
}

void
CodeGeneratorLLVMContext::generate_operation_subtract(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationBinary & operation
    )
{
    size_t a = operation.get_a();
    size_t b = operation.get_b();
    const Gyoji::mir::Type *atype = mir_function.tmpvar_get(a);
    const Gyoji::mir::Type *btype = mir_function.tmpvar_get(b);
    if (!atype->is_numeric() || !btype->is_numeric()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for subtract operator.",
		std::string("Invalid operands for subtract operation.  Operand must be a numeric type, but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
    llvm::Value *value_a = tmp_values[a];
    llvm::Value *value_b = tmp_values[b];

    if (atype->is_integer() && btype->is_integer()) {
	llvm::Value *sum = Builder->CreateSub(value_a, value_b);
	tmp_values.insert(std::pair(operation.get_result(), sum));
    }
    else if (atype->is_float() && btype->is_float()) {
	llvm::Value *sum = Builder->CreateFSub(value_a, value_b);
	tmp_values.insert(std::pair(operation.get_result(), sum));
    }
    else {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for subtract operator.",
		std::string("Operands must be both integer or both floating-point but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
}

void
CodeGeneratorLLVMContext::generate_operation_multiply(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationBinary & operation
    )
{
    size_t a = operation.get_a();
    size_t b = operation.get_b();
    const Gyoji::mir::Type *atype = mir_function.tmpvar_get(a);
    const Gyoji::mir::Type *btype = mir_function.tmpvar_get(b);
    if (!atype->is_numeric() || !atype->is_numeric()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for add operator.",
		std::string("Invalid operands for add operation.  Operand must be a numeric type, but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
    llvm::Value *value_a = tmp_values[a];
    llvm::Value *value_b = tmp_values[b];

    if (atype->is_integer() && atype->is_integer()) {
	llvm::Value *sum = Builder->CreateMul(value_a, value_b);
	tmp_values.insert(std::pair(operation.get_result(), sum));
    }
    else if (atype->is_float() && atype->is_float()) {
	llvm::Value *sum = Builder->CreateFMul(value_a, value_b);
	tmp_values.insert(std::pair(operation.get_result(), sum));
    }
    else {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for add operator.",
		std::string("Operands must be both integer or floating-point primitive types but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
}

void
CodeGeneratorLLVMContext::generate_operation_divide(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationBinary & operation
    )
{
    size_t a = operation.get_a();
    size_t b = operation.get_b();
    const Gyoji::mir::Type *atype = mir_function.tmpvar_get(a);
    const Gyoji::mir::Type *btype = mir_function.tmpvar_get(b);
    if (!atype->is_numeric() || !btype->is_numeric()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for divide operator.",
		std::string("Invalid operands for divide operation.  Operand must be a numeric type, but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
    llvm::Value *value_a = tmp_values[a];
    llvm::Value *value_b = tmp_values[b];

    if (atype->is_integer() && atype->is_integer()) {
	if (atype->is_signed() && btype->is_signed()) {
	    llvm::Value *sum = Builder->CreateSDiv(value_a, value_b);
	    tmp_values.insert(std::pair(operation.get_result(), sum));
	}
	else if (atype->is_unsigned() && btype->is_unsigned()) {
	    llvm::Value *sum = Builder->CreateUDiv(value_a, value_b);
	    tmp_values.insert(std::pair(operation.get_result(), sum));
	}
	else {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    operation.get_source_ref(),
		    "Compiler bug! Invalid operand for divide operator.",
		    std::string("Operands must be both signed or unsigned integers but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		    );
	}
    }
    else if (atype->is_float()) {
	llvm::Value *sum = Builder->CreateFDiv(value_a, value_b);
	tmp_values.insert(std::pair(operation.get_result(), sum));
    }
    else {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for divide operator.",
		std::string("Operands must be integer or floating-point primitive types but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
}
void
CodeGeneratorLLVMContext::generate_operation_modulo(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationBinary & operation
    )
{
    size_t a = operation.get_a();
    size_t b = operation.get_b();
    const Gyoji::mir::Type *atype = mir_function.tmpvar_get(a);
    const Gyoji::mir::Type *btype = mir_function.tmpvar_get(b);
    if (!atype->is_integer() || !btype->is_integer()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for modulo operator.",
		std::string("Invalid operands for modulo operation.  Operand must be a integer type, but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
    llvm::Value *value_a = tmp_values[a];
    llvm::Value *value_b = tmp_values[b];

    if (atype->is_signed() && btype->is_signed()) {
	llvm::Value *sum = Builder->CreateSRem(value_a, value_b);
	tmp_values.insert(std::pair(operation.get_result(), sum));
    }
    else if (atype->is_unsigned() && btype->is_unsigned()) {
	llvm::Value *sum = Builder->CreateURem(value_a, value_b);
	tmp_values.insert(std::pair(operation.get_result(), sum));
    }
    else {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for modulo operator.",
		std::string("Invalid operands for modulo operation.  Operand must be a both signed or both unsigned, but were ") +
		    atype->get_name() + std::string(" and ") + btype->get_name()
		);
    }
}

// Binary operations: logical
void
CodeGeneratorLLVMContext::generate_operation_logical_and(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationBinary & operation
    )
{
    size_t a = operation.get_a();
    size_t b = operation.get_b();
    const Gyoji::mir::Type *atype = mir_function.tmpvar_get(a);
    const Gyoji::mir::Type *btype = mir_function.tmpvar_get(b);
    if (!atype->is_bool() || !btype->is_bool()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for logical and operator.",
		std::string("Invalid operands for and operation.  Operand must be a bool type, but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
    llvm::Value *value_a = tmp_values[a];
    llvm::Value *value_b = tmp_values[b];

    llvm::Value *sum = Builder->CreateAnd(value_a, value_b);
    tmp_values.insert(std::pair(operation.get_result(), sum));

}

void
CodeGeneratorLLVMContext::generate_operation_logical_or(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationBinary & operation
    )
{
    size_t a = operation.get_a();
    size_t b = operation.get_b();
    const Gyoji::mir::Type *atype = mir_function.tmpvar_get(a);
    const Gyoji::mir::Type *btype = mir_function.tmpvar_get(b);
    if (!atype->is_bool() || !btype->is_bool()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for logical or operator.",
		std::string("Invalid operands for logical or operation.  Operands must be bool, but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
    llvm::Value *value_a = tmp_values[a];
    llvm::Value *value_b = tmp_values[b];

    llvm::Value *sum = Builder->CreateOr(value_a, value_b);
    tmp_values.insert(std::pair(operation.get_result(), sum));
}


// Binary operations: bitwise
void
CodeGeneratorLLVMContext::generate_operation_bitwise_and(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationBinary & operation
    )
{
    size_t a = operation.get_a();
    size_t b = operation.get_b();
    const Gyoji::mir::Type *atype = mir_function.tmpvar_get(a);
    const Gyoji::mir::Type *btype = mir_function.tmpvar_get(b);
    if (!atype->is_unsigned() || !btype->is_unsigned()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for bitwise and operator.",
		std::string("Invalid operands for logical and operation.  Operands must be unsigned, but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
    llvm::Value *value_a = tmp_values[a];
    llvm::Value *value_b = tmp_values[b];

    llvm::Value *sum = Builder->CreateAnd(value_a, value_b);
    tmp_values.insert(std::pair(operation.get_result(), sum));
}

void
CodeGeneratorLLVMContext::generate_operation_bitwise_or(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationBinary & operation
    )
{
    size_t a = operation.get_a();
    size_t b = operation.get_b();
    const Gyoji::mir::Type *atype = mir_function.tmpvar_get(a);
    const Gyoji::mir::Type *btype = mir_function.tmpvar_get(b);
    if (!atype->is_unsigned() || !btype->is_unsigned()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for bitwise or operator.",
		std::string("Invalid operands for logical or operation.  Operands must be unsigned, but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
    llvm::Value *value_a = tmp_values[a];
    llvm::Value *value_b = tmp_values[b];

    llvm::Value *sum = Builder->CreateOr(value_a, value_b);
    tmp_values.insert(std::pair(operation.get_result(), sum));
}

void
CodeGeneratorLLVMContext::generate_operation_bitwise_xor(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationBinary & operation
    )
{
    size_t a = operation.get_a();
    size_t b = operation.get_b();
    const Gyoji::mir::Type *atype = mir_function.tmpvar_get(a);
    const Gyoji::mir::Type *btype = mir_function.tmpvar_get(b);
    if (!atype->is_unsigned() || !btype->is_unsigned()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for bitwise xor operator.",
		std::string("Invalid operands for logical xor operation.  Operands must be unsigned, but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
    llvm::Value *value_a = tmp_values[a];
    llvm::Value *value_b = tmp_values[b];

    llvm::Value *sum = Builder->CreateXor(value_a, value_b);
    tmp_values.insert(std::pair(operation.get_result(), sum));
}

void
CodeGeneratorLLVMContext::generate_operation_shift(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationBinary & operation
    )
{
    size_t a = operation.get_a();
    size_t b = operation.get_b();
    const Gyoji::mir::Type *atype = mir_function.tmpvar_get(a);
    const Gyoji::mir::Type *btype = mir_function.tmpvar_get(b);
    if (!atype->is_unsigned() || !btype->is_unsigned()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for bitwise shift left operator.",
		std::string("Invalid operands for logical shift left operation.  Operands must be unsigned, but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
    llvm::Value *value_a = tmp_values[a];
    llvm::Value *value_b = tmp_values[b];

    unsigned short mask = 0x07;
    size_t a_size = atype->get_primitive_size();
    switch (a_size) {
    case 8:
	mask = 0x07;
	break;
    case 16:
	mask = 0x0f;
	break;
    case 32:
	mask = 0x1f;
	break;
    case 64:
	mask = 0x3f;
	break;
    default:
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for bitwise shift left operator.",
		std::string("Invalid operands for logical shift left operation.  Left operand must be 8, 16, 32, or 64 bits but operands were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }

    // Here, if the right-hand argument
    // must be less than the number of bits in the
    // right hand argument, so in order to make sure
    // that's the case, we need to mask the right-hand
    // argument appropriately.
    llvm::Value *shr_bits_mask = Builder->getInt8(mask);
    llvm::Value *shr_bits = Builder->CreateAnd(value_b, shr_bits_mask);
    llvm::Value *shifted_value;
    if (operation.get_type() == Operation::OP_SHIFT_LEFT) {
	shifted_value = Builder->CreateShl(value_a, value_b);
    }
    else {
	shifted_value = Builder->CreateLShr(value_a, shr_bits);
    }
    tmp_values.insert(std::pair(operation.get_result(), shifted_value));
}

// Binary operations: comparisons
void
CodeGeneratorLLVMContext::generate_operation_comparison(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationBinary & operation
    )
{
    size_t a = operation.get_a();
    size_t b = operation.get_b();
    Operation::OperationType type = operation.get_type();
    
    const Gyoji::mir::Type *atype = mir_function.tmpvar_get(a);
    const Gyoji::mir::Type *btype = mir_function.tmpvar_get(b);
    if (atype->get_name() != btype->get_name()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for comparison operator.",
		std::string("Invalid operands for comparison operation.  Operands must be the same type, but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
    if (atype->is_void()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for comparison operation.",
		std::string("The operands of a comparison must not be void, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return;
    }
    if (atype->is_composite()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for comparison operation.",
		std::string("The operands of a comparison must not be composite structures or classes, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return;
    }
    if (
	(atype->is_pointer() || atype->is_reference())
	&&
	!(type == Operation::OP_COMPARE_EQUAL ||
	  type == Operation::OP_COMPARE_NOT_EQUAL)
	) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for comparison operation.",
		std::string("The operands of a comparison of pointers and references may not be used except for equality comparisions, but were: a= ") +
		atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return;
    }
    
    llvm::Value *value_a = tmp_values[a];
    llvm::Value *value_b = tmp_values[b];

    llvm::Value * result = nullptr;

    switch (type) {
    case Operation::OP_COMPARE_LESS:
	if (atype->is_float()) {
	    result = Builder->CreateFCmpULT(value_a, value_b);
	}
	else if (atype->is_signed()) {
	    result = Builder->CreateICmpSLT(value_a, value_b);
	}
	else {
	    result = Builder->CreateICmpULT(value_a, value_b);
	}
	break;
    case Operation::OP_COMPARE_GREATER:
	if (atype->is_float()) {
	    result = Builder->CreateFCmpUGT(value_a, value_b);
	}
	else if (atype->is_signed()) {
	    result = Builder->CreateICmpSGT(value_a, value_b);
	}
	else {
	    result = Builder->CreateICmpUGT(value_a, value_b);
	}
	break;
    case Operation::OP_COMPARE_LESS_EQUAL:
	if (atype->is_float()) {
	    result = Builder->CreateFCmpULE(value_a, value_b);
	}
	else if (atype->is_signed()) {
	    result = Builder->CreateICmpSLE(value_a, value_b);
	}
	else {
	    result = Builder->CreateICmpULE(value_a, value_b);
	}
	break;
    case Operation::OP_COMPARE_GREATER_EQUAL:
	if (atype->is_float()) {
	    result = Builder->CreateFCmpUGE(value_a, value_b);
	}
	else if (atype->is_signed()) {
	    result = Builder->CreateICmpSGE(value_a, value_b);
	}
	else {
	    result = Builder->CreateICmpUGE(value_a, value_b);
	}
	break;
    case Operation::OP_COMPARE_EQUAL:
	if (atype->is_float()) {
	    result = Builder->CreateFCmpUEQ(value_a, value_b);
	}
	else {
	    result = Builder->CreateICmpEQ(value_a, value_b);
	}
	break;
    case Operation::OP_COMPARE_NOT_EQUAL:
	if (atype->is_float()) {
	    result = Builder->CreateFCmpUNE(value_a, value_b);
	}
	else {
	    result = Builder->CreateICmpNE(value_a, value_b);
	}
	break;
    default:
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation.get_source_ref(),
		"Compiler bug! Invalid operand for comparison operation.",
		std::string("Unknown operand type")
		);
    }
    tmp_values.insert(std::pair(operation.get_result(), result));
}

// Binary operations: assignments
void
CodeGeneratorLLVMContext::generate_operation_assign(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationBinary & operation
    )
{
    const Type *atype = mir_function.tmpvar_get(operation.get_a());
    const Type *btype = mir_function.tmpvar_get(operation.get_b());
    // We're allowed to assign between pointers and references
    // fairly interchangably.  The lowering rules check
    // whether this is allowed by the 'unsafe' blocks.
    if (
	(atype->is_reference() && btype->is_pointer()) ||
	(atype->is_pointer() && btype->is_reference())
	) {
	llvm::Value * a_value = tmp_values[operation.get_a()];
	llvm::Value * a_lvalue = tmp_lvalues[operation.get_a()];
	llvm::Value * b_value = tmp_values[operation.get_b()];
	llvm::Value *assigned_value = Builder->CreateStore(b_value, a_lvalue);
	tmp_values.insert(std::pair(operation.get_result(), a_value));
	tmp_lvalues.insert(std::pair(operation.get_result(), a_lvalue));
    }
    else {
	llvm::Value * a_lvalue = tmp_lvalues[operation.get_a()];
	llvm::Value * b_value = tmp_values[operation.get_b()];
	llvm::Value *assigned_value = Builder->CreateStore(b_value, a_lvalue);
	
	// TODO: Assigning an lvalue results in an lvalue.
	const auto & b_lvalue = tmp_lvalues.find(operation.get_b());
	if (b_lvalue != tmp_lvalues.end()) {
	    tmp_lvalues.insert(std::pair(operation.get_result(), b_lvalue->second));
	}
	
	tmp_values.insert(std::pair(operation.get_result(), b_value));
    }
}

// Branch and flow control
void
CodeGeneratorLLVMContext::generate_operation_jump_conditional(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationJumpConditional & operation
    )
{
    llvm::Value *condition = tmp_values[operation.get_operands().at(0)];
    llvm::BasicBlock *bbIf = blocks[operation.get_if_block()];
    llvm::BasicBlock *bbElse = blocks[operation.get_else_block()];
    Builder->CreateCondBr(condition, bbIf, bbElse);
}
void
CodeGeneratorLLVMContext::generate_operation_jump(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationJump & operation
    )
{
    llvm::BasicBlock *target = blocks[operation.get_jump_block()];
    Builder->CreateBr(target);
}

llvm::Value *
CodeGeneratorLLVMContext::generate_operation_return(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationReturn & operation
    )
{
    llvm::Value *value = tmp_values[operation.get_operands().at(0)];
    Builder->CreateRet(value);
    return value;
}

llvm::Value *
CodeGeneratorLLVMContext::generate_operation_return_void(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::OperationReturnVoid & operation
    )
{
    fprintf(stderr, "Returning void\n");
    llvm::Value *value = tmp_values[operation.get_operands().at(0)];
    Builder->CreateRetVoid();
    return value;
}

llvm::Value *
CodeGeneratorLLVMContext::generate_basic_block(
    const Gyoji::mir::Function & mir_function,
    const Gyoji::mir::BasicBlock & mir_block
    )
{
    llvm::Value *return_value = nullptr;

    for (const auto & operation_el : mir_block.get_operations()) {
	const Operation & operation = *operation_el;
	switch (operation.get_type()) {
        // Global symbols
	case Operation::OP_FUNCTION_CALL:
	    generate_operation_function_call(mir_function, (const OperationFunctionCall &)operation);
	    break;
	case Operation::OP_GET_METHOD:
	    generate_operation_get_method(mir_function, (const OperationGetMethod&)operation);
	    break;
	case Operation::OP_METHOD_GET_OBJECT:
	    generate_operation_method_get_object(mir_function, (const OperationUnary&)operation);
	    break;
	case Operation::OP_METHOD_GET_FUNCTION:
	    generate_operation_method_get_function(mir_function, (const OperationUnary&)operation);
	    break;
	case Operation::OP_SYMBOL:
	    generate_operation_symbol(mir_function, (const OperationSymbol &)operation);
	    break;
        // Cast operations
	case Operation::OP_WIDEN_SIGNED:
	case Operation::OP_WIDEN_UNSIGNED:
	case Operation::OP_WIDEN_FLOAT:
	    generate_operation_widen_numeric(mir_function, (const OperationCast &)operation);
	    break;
        // Indirect access
	case Operation::OP_ARRAY_INDEX:
	    generate_operation_array_index(mir_function, (const OperationArrayIndex &)operation);
	    break;
	case Operation::OP_DOT:
	    generate_operation_dot(mir_function, (const OperationDot &)operation);
	    break;
	case Operation::OP_LOCAL_VARIABLE:
	    generate_operation_local_variable(mir_function, (const OperationLocalVariable &)operation);
	    break;
	case Operation::OP_LOCAL_DECLARE:
	    generate_operation_local_declare(mir_function, (const OperationLocalDeclare &)operation);
	    break;
	case Operation::OP_LOCAL_UNDECLARE:
	    generate_operation_local_undeclare(mir_function, (const OperationLocalUndeclare &)operation);
	    break;
	case Operation::OP_LITERAL_CHAR:
	    generate_operation_literal_char(mir_function, (const OperationLiteralChar &)operation);
	    break;
	case Operation::OP_LITERAL_STRING:
	    generate_operation_literal_string(mir_function, (const OperationLiteralString &)operation);
	    break;
	case Operation::OP_LITERAL_INT:
	    generate_operation_literal_int(mir_function, (const OperationLiteralInt &)operation);
	    break;
	case Operation::OP_LITERAL_FLOAT:
	    generate_operation_literal_float(mir_function, (const OperationLiteralFloat &)operation);
	    break;
	case Operation::OP_LITERAL_BOOL:
	    generate_operation_literal_bool(mir_function, (const OperationLiteralBool &)operation);
	    break;
	case Operation::OP_LITERAL_NULL:
	    generate_operation_literal_null(mir_function, (const OperationLiteralNull &)operation);
	    break;
	    
        // Unary operations	    
	case Operation::OP_ADDRESSOF:
	    generate_operation_addressof(mir_function, (const OperationUnary &)operation);
	    break;
	case Operation::OP_DEREFERENCE:
	    generate_operation_dereference(mir_function, (const OperationUnary &)operation);
	    break;
	case Operation::OP_NEGATE:
	    generate_operation_arithmetic_negate(mir_function, (const OperationUnary &)operation);
	    break;
	case Operation::OP_BITWISE_NOT:
	    generate_operation_bitwise_not(mir_function, (const OperationUnary &)operation);
	    break;
	case Operation::OP_LOGICAL_NOT:
	    generate_operation_logical_not(mir_function, (const OperationUnary &)operation);
	    break;
        // Binary operations
	case Operation::OP_ADD:
	    generate_operation_add(mir_function, (const OperationBinary &)operation);
	    break;
	case Operation::OP_SUBTRACT:
	    generate_operation_subtract(mir_function, (const OperationBinary &)operation);
	    break;
	case Operation::OP_MULTIPLY:
	    generate_operation_multiply(mir_function, (const OperationBinary &)operation);
	    break;
	case Operation::OP_DIVIDE:
	    generate_operation_divide(mir_function, (const OperationBinary &)operation);
	    break;
	case Operation::OP_MODULO:
	    generate_operation_modulo(mir_function, (const OperationBinary &)operation);
	    break;
	case Operation::OP_LOGICAL_AND:
	    generate_operation_logical_and(mir_function, (const OperationBinary &)operation);
	    break;
	case Operation::OP_LOGICAL_OR:
	    generate_operation_logical_or(mir_function, (const OperationBinary &)operation);
	    break;
	case Operation::OP_BITWISE_AND:
	    generate_operation_bitwise_and(mir_function, (const OperationBinary &)operation);
	    break;
	case Operation::OP_BITWISE_OR:
	    generate_operation_bitwise_xor(mir_function, (const OperationBinary &)operation);
	    break;
	case Operation::OP_BITWISE_XOR:
	    generate_operation_bitwise_xor(mir_function, (const OperationBinary &)operation);
	    break;
	case Operation::OP_SHIFT_LEFT:
	case Operation::OP_SHIFT_RIGHT:
	    generate_operation_shift(mir_function, (const OperationBinary &)operation);
	    break;
        // Binary operations: comparisons
	case Operation::OP_COMPARE_LESS:
	case Operation::OP_COMPARE_GREATER:
	case Operation::OP_COMPARE_LESS_EQUAL:
	case Operation::OP_COMPARE_GREATER_EQUAL:
	case Operation::OP_COMPARE_EQUAL:
	case Operation::OP_COMPARE_NOT_EQUAL:
	    generate_operation_comparison(mir_function, (const OperationBinary &)operation);
	    break;
	case Operation::OP_ASSIGN:
	    generate_operation_assign(mir_function, (const OperationBinary &)operation);
	    break;
	case Operation::OP_SIZEOF_TYPE:
	    generate_operation_sizeof_type(mir_function, (const OperationSizeofType &)operation);
	    break;
	case Operation::OP_JUMP_CONDITIONAL:
	    generate_operation_jump_conditional(mir_function, (const OperationJumpConditional &)operation);
	    break;
	case Operation::OP_JUMP:
	    generate_operation_jump(mir_function, (const OperationJump &)operation);
	    break;
	case Operation::OP_RETURN:
	    return_value = generate_operation_return(mir_function, (const OperationReturn &)operation);
	    break;
	case Operation::OP_RETURN_VOID:
	    return_value = generate_operation_return_void(mir_function, (const OperationReturnVoid &)operation);
	    break;
	}
    }
    return return_value;
}


void
CodeGeneratorLLVMContext::generate_function(const Gyoji::mir::Function & function)
{
    // TODO: We should probably break this stuff off
    // into a separate object so we don't have to manually
    // clear context like this.
    local_lvalues.clear();
    local_variables.clear();
    blocks.clear();
    tmp_values.clear();
    tmp_lvalues.clear();
    
    // Transfer ownership of the prototype to the FunctionProtos map, but keep a
    // reference to it for use below.
    llvm::Function *TheFunction = create_function(function);
    if (!TheFunction) {
	fprintf(stderr, "Function declaration not found\n");
    }

    // Record the function arguments in the NamedValues map.
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*TheContext, "entry", TheFunction);
    Builder->SetInsertPoint(BB);

    size_t i = 0;
    for (const auto & function_argument : function.get_arguments()) {
	// Create an alloca for this variable.
	llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
			       TheFunction->getEntryBlock().begin());
	llvm::AllocaInst *argument_alloca = TmpB.CreateAlloca(
	    types[function_argument.get_type()->get_name()],
	    nullptr,
	    function_argument.get_name()
	    );

	llvm::Argument *arg = TheFunction->getArg(i);
	Builder->CreateStore(arg, argument_alloca);
	
	// Add arguments to variable symbol table.
	local_variables[function_argument.get_name()] = argument_alloca;
	i++;
    }

    for (const auto & block_it : function.get_blocks()) {
	// Skip empty blocks.
	if (block_it.second->size() == 0) {
	    continue;
	}
	
	std::string block_name = std::string("BB") + std::to_string(block_it.first);
	llvm::BasicBlock *BB = llvm::BasicBlock::Create(*TheContext, block_name, TheFunction);
	blocks[block_it.first] = BB;
    }
    // Jump from the entry block into the first 'real' block.
    Builder->CreateBr(blocks[0]);
    
    for (const auto & block_it : function.get_blocks()) {
	// Skip empty blocks.
	// We've already verified in the analysis phase
	// that if a block is unreachable, it's also empty.
	if (block_it.second->size() == 0) {
	    continue;
	}
	
	// Create a new basic block to start insertion into.
	llvm::BasicBlock *BB = blocks[block_it.first];
	Builder->SetInsertPoint(BB);
	generate_basic_block(function, *block_it.second);
    }
    
    // Validate the generated code, checking for consistency.
    verifyFunction(*TheFunction);
    
    local_variables.clear();
}


int
CodeGeneratorLLVMContext::output(const std::string & filename)
{
    using namespace llvm;
    // Initialize the target registry etc.
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();
    
    auto TargetTriple = sys::getDefaultTargetTriple();
    TheModule->setTargetTriple(TargetTriple);
    
    std::string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);
    
    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!Target) {
	errs() << Error;
	return 1;
    }
    
    auto CPU = "generic";
    auto Features = "";
    
    TargetOptions opt;
    auto TheTargetMachine = Target->createTargetMachine(
	TargetTriple, CPU, Features, opt, Reloc::PIC_);
    
    TheModule->setDataLayout(TheTargetMachine->createDataLayout());
    
    std::error_code EC;
    raw_fd_ostream dest(filename, EC, sys::fs::OF_None);
    
    if (EC) {
	errs() << "Could not open file: " << EC.message();
	return 1;
    } 

    llvm::raw_fd_ostream  llvm_ll_ostream(filename + std::string(".ll"), EC);
    TheModule->print(llvm_ll_ostream, nullptr);

    // For now, until we have a cmdline parser
    // we will default to no optimization so we
    // can actually follow the generated assembly.
    // When we want to run a 'real' build, we will
    // want to turn on better optimizations.
    TheTargetMachine->setOptLevel(CodeGenOptLevel::None);
    
    legacy::PassManager pass;
    auto FileType = CodeGenFileType::ObjectFile;
    
    if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
	errs() << "TheTargetMachine can't emit a file of this type";
	return 1;
    }
    
    pass.run(*TheModule);
    dest.flush();
    
    outs() << "Wrote " << filename << "\n";
    
    return 0;
}
