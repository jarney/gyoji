#include <jlang-codegen.hpp>
#include "jlang-codegen-private.hpp"

using namespace llvm::sys;
using namespace JLang::codegen;
using namespace JLang::mir;

CodeGeneratorLLVM::CodeGeneratorLLVM(
    const JLang::context::CompilerContext & _compiler_context,
    const JLang::mir::MIR & _mir
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
    const JLang::context::CompilerContext & _compiler_context,
    const JLang::mir::MIR & _mir
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
    TheModule = std::make_unique<llvm::Module>("jlang LLVM Code Generator", *TheContext);
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
    
    llvm::Function *F =
	llvm::Function::Create(FT, llvm::Function::ExternalLinkage, function.get_name(), TheModule.get());
    
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
CodeGeneratorLLVMContext::create_type_enum(const JLang::mir::Type *enumtype)
{
    return llvm::Type::getInt32Ty(*TheContext);
}

llvm::Type *
CodeGeneratorLLVMContext::create_type_composite(const JLang::mir::Type *compositetype)
{
    std::vector<llvm::Type*> members;
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
CodeGeneratorLLVMContext::create_type_pointer(const JLang::mir::Type *pointertype)
{
    const JLang::mir::Type * pointer_target = pointertype->get_pointer_target();
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
CodeGeneratorLLVMContext::create_type_reference(const JLang::mir::Type *referencetype)
{
    const JLang::mir::Type * pointer_target = referencetype->get_pointer_target();
    llvm::Type * llvm_type =
	llvm::PointerType::get(create_type(pointer_target),
			       0 // Address space (default to 0?  This seems unclean, llvm!)
	    );
    types.insert(std::pair<std::string, llvm::Type*>(
		     referencetype->get_name(),
		     llvm_type
		     )
	);
    return llvm_type;  return llvm_type;
}

llvm::Type *
CodeGeneratorLLVMContext::create_type_function_pointer(const JLang::mir::Type *fptr_type)
{
    const JLang::mir::Type *mir_return_type = fptr_type->get_return_type();
    llvm::Type *llvm_return_type = create_type(mir_return_type);
    
    const std::vector<JLang::mir::Argument> & mir_args = fptr_type->get_argument_types();
    
    std::vector<llvm::Type *> llvm_fptr_args;
    for (const auto & mir_arg : mir_args) {
	llvm_fptr_args.push_back(create_type(mir_arg.get_type()));
    }
    
    llvm::ArrayRef<llvm::Type *> llvm_args_ref(llvm_fptr_args);
    llvm::FunctionType *llvm_fptr_type = llvm::FunctionType::get(llvm_return_type, llvm_args_ref, false);
    
    return llvm_fptr_type;
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
    
    if (primitive->get_name() == "i8") {
	llvm_type = llvm::Type::getInt8Ty(*TheContext);
    }
    // Signed integer types
    else if (primitive->get_name() == "i16") {
	llvm_type = llvm::Type::getInt16Ty(*TheContext);
    }
    else if (primitive->get_name() == "i32") {
	llvm_type = llvm::Type::getInt32Ty(*TheContext);
    }
    else if (primitive->get_name() == "i64") {
	llvm_type = llvm::Type::getInt64Ty(*TheContext);
    }
    // Unsigned integer types
    else if (primitive->get_name() == "u8") {
	llvm_type = llvm::Type::getInt8Ty(*TheContext);
    }
    else if (primitive->get_name() == "u16") {
	llvm_type = llvm::Type::getInt16Ty(*TheContext);
    }
    else if (primitive->get_name() == "u32") {
	llvm_type = llvm::Type::getInt32Ty(*TheContext);
    }
    else if (primitive->get_name() == "u64") {
	llvm_type = llvm::Type::getInt64Ty(*TheContext);
    }
    // Floating-point types.
    else if (primitive->get_name() == "f32") {
	llvm_type = llvm::Type::getFloatTy(*TheContext);
    }
    else if (primitive->get_name() == "f64") {
	llvm_type = llvm::Type::getDoubleTy(*TheContext);
    }
    // "Special" types
    else if (primitive->get_name() == "void") {
	llvm_type = llvm::Type::getVoidTy(*TheContext);
    }
    else if (primitive->get_name() == "bool") {
	// Bool is just a u32 under the covers.
	llvm_type = llvm::Type::getInt32Ty(*TheContext);
    }
    // Other (unknown, this must be a bug)
    else {
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
    
    Type::TypeType t = type->get_type();
    llvm::Type* llvm_type;
    if (t == Type::TYPE_PRIMITIVE) {
	return create_type_primitive(type);
    }
    else if (t == Type::TYPE_ENUM) {
	return create_type_enum(type);
    }
    else if (t == Type::TYPE_COMPOSITE) {
	return create_type_composite(type);
    }
    else if (t == Type::TYPE_POINTER) {
	return create_type_pointer(type);
    }
    else if (t == Type::TYPE_REFERENCE) {
	return create_type_reference(type);
    }
    else if (t == Type::TYPE_FUNCTION_POINTER) {
	return create_type_function_pointer(type);
    }
    fprintf(stderr, "Compiler BUG!  Unknown type type passed to code generator %d\n", t);
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
	fprintf(stderr, "Generating for function %s\n", function->get_name().c_str());
	generate_function(*function);
    }
}

void
CodeGeneratorLLVMContext::generate_operation_function_call(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationFunctionCall *operation
    )
{
}
void
CodeGeneratorLLVMContext::generate_operation_symbol(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationSymbol *operation
    )
{}
void
CodeGeneratorLLVMContext::generate_operation_local_variable(
    std::map<size_t, llvm::Value *> & tmp_values,
    std::map<size_t, llvm::Value *> & tmp_lvalues,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationLocalVariable *operation
    )
{
    llvm::Type *type = types[operation->get_var_type()->get_name()];
    llvm::Value *variable_ptr = local_variables[operation->get_symbol_name()];
    llvm::Value *value = Builder->CreateLoad(type, variable_ptr);
    tmp_lvalues.insert(std::pair(operation->get_result(), variable_ptr));
    tmp_values.insert(std::pair(operation->get_result(), value));
    fprintf(stderr, "%ld = local var %s\n",
	    operation->get_result(),
	    operation->get_symbol_name().c_str());
}
void
CodeGeneratorLLVMContext::generate_operation_local_declare(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationLocalDeclare *operation
    )
{
    llvm::Type *type = types[operation->get_var_type()];
    llvm::Value *value = Builder->CreateAlloca(type, nullptr, operation->get_variable());
    local_variables[operation->get_variable()] = value;
}
void
CodeGeneratorLLVMContext::generate_operation_local_undeclare(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationLocalUndeclare *operation
    )
{}
void
CodeGeneratorLLVMContext::generate_operation_literal_char(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationLiteralChar *operation
    )
{
}
void
CodeGeneratorLLVMContext::generate_operation_literal_string(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationLiteralString *operation
    )
{}
void
CodeGeneratorLLVMContext::generate_operation_literal_int(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationLiteralInt *operation
    )
{
    llvm::Value *value = Builder->getInt32(atol(operation->get_literal_int().c_str()));
    fprintf(stderr, "Literal int goes into %ld\n", operation->get_result());
    tmp_values.insert(std::pair(operation->get_result(), value));
}
void
CodeGeneratorLLVMContext::generate_operation_literal_float(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationLiteralFloat *operation
    )
{}
void
CodeGeneratorLLVMContext::generate_operation_post_increment(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationUnary *operation
    )
{}
void
CodeGeneratorLLVMContext::generate_operation_post_decrement(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationUnary *operation
    )
{}
void
CodeGeneratorLLVMContext::generate_operation_pre_increment(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationUnary *operation
    )
{}
void
CodeGeneratorLLVMContext::generate_operation_pre_decrement(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationUnary *operation
    )
{}
void
CodeGeneratorLLVMContext::generate_operation_add(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationBinary *operation
    )
{
    size_t a = operation->get_a();
    size_t b = operation->get_b();
    const JLang::mir::Type *atype = mir_function.tmpvar_get(a)->get_type();
    const JLang::mir::Type *btype = mir_function.tmpvar_get(b)->get_type();
    if ((atype->get_type() != JLang::mir::Type::TYPE_PRIMITIVE) ||
	(btype->get_type() != JLang::mir::Type::TYPE_PRIMITIVE)) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation->get_source_ref(),
		"Compiler bug! Invalid operand for add operator.",
		std::string("Invalid operands for add operation.  Operand must be a numeric type, but was ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
    if (atype->get_name() != btype->get_name()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation->get_source_ref(),
		"Compiler bug! Invalid operand for add operator.",
		std::string("Operands must match ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
    
    llvm::Value *value_a = tmp_values[operation->get_a()];
    llvm::Value *value_b = tmp_values[operation->get_b()];

    std::string primitive_name = atype->get_name();
    if (primitive_name == std::string("u8") ||
	primitive_name == std::string("u16") ||
	primitive_name == std::string("u32") ||
	primitive_name == std::string("u64") ||
	primitive_name == std::string("i8") ||
	primitive_name == std::string("i16") ||
	primitive_name == std::string("i32") ||
	primitive_name == std::string("i64")) {
	llvm::Value *sum = Builder->CreateAdd(value_a, value_b);
	tmp_values.insert(std::pair(operation->get_result(), sum));
    }
    else if (primitive_name == std::string("f32") ||
	     primitive_name == std::string("f64")) {
	llvm::Value *sum = Builder->CreateFAdd(value_a, value_b);
	tmp_values.insert(std::pair(operation->get_result(), sum));
    }
    else {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		operation->get_source_ref(),
		"Compiler bug! Invalid operand for add operator.",
		std::string("Operands must be integer or floating-point primitive types but were ") + atype->get_name() + std::string(" and ") + btype->get_name()
		);
	return;
    }
}

void
CodeGeneratorLLVMContext::generate_operation_subtract(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationBinary *operation
    )
{}
void
CodeGeneratorLLVMContext::generate_operation_multiply(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationBinary *operation
    )
{}
void
CodeGeneratorLLVMContext::generate_operation_divide(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationBinary *operation
    )
{}
void
CodeGeneratorLLVMContext::generate_operation_assign(
    std::map<size_t, llvm::Value *> & tmp_values,
    std::map<size_t, llvm::Value *> & tmp_lvalues,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationBinary *operation
    )
{
    llvm::Value *lvalue = tmp_lvalues[operation->get_operands().at(0)];
    llvm::Value *value_b = tmp_values[operation->get_operands().at(1)];
    llvm::Value *value = Builder->CreateStore(value_b, lvalue);
    tmp_values.insert(std::pair(operation->get_result(), value));
}
void
CodeGeneratorLLVMContext::generate_operation_jump_if_equal(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationJumpIfEqual *operation
    )
{}
void
CodeGeneratorLLVMContext::generate_operation_jump(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationJump *operation
    )
{}

llvm::Value *
CodeGeneratorLLVMContext::generate_operation_return(
    std::map<size_t, llvm::Value *> & tmp_values,
    const JLang::mir::Function & mir_function,
    const JLang::mir::OperationReturn *operation
    )
{
    llvm::Value *value = tmp_values[operation->get_operands().at(0)];
    Builder->CreateRet(value);
    return value;
}

llvm::Value *
CodeGeneratorLLVMContext::generate_basic_block(
    const JLang::mir::Function & mir_function,
    size_t blockid
    )
{
    fprintf(stderr, "Generating code for block BB%ld\n", blockid);
    const JLang::mir::BasicBlock & mir_block = mir_function.get_basic_block(blockid);
    std::map<size_t, llvm::Value *> tmp_values;
    std::map<size_t, llvm::Value *> tmp_lvalues;

    llvm::Value *return_value = nullptr;

    for (const auto & operation : mir_block.get_operations()) {
	switch (operation->get_type()) {
	case Operation::OP_FUNCTION_CALL:
	    generate_operation_function_call(tmp_values, mir_function, (JLang::mir::OperationFunctionCall*)operation.get());
	    break;
	case Operation::OP_SYMBOL:
	    generate_operation_symbol(tmp_values, mir_function, (OperationSymbol*)operation.get());
	    break;
	case Operation::OP_LOCAL_VARIABLE:
	    generate_operation_local_variable(tmp_values, tmp_lvalues, mir_function, (OperationLocalVariable*)operation.get());
	    break;
	case Operation::OP_LOCAL_DECLARE:
	    generate_operation_local_declare(tmp_values, mir_function, (OperationLocalDeclare*)operation.get());
	    break;
	case Operation::OP_LOCAL_UNDECLARE:
	    generate_operation_local_undeclare(tmp_values, mir_function, (OperationLocalUndeclare*)operation.get());
	    break;
	case Operation::OP_LITERAL_CHAR:
	    generate_operation_literal_char(tmp_values, mir_function, (OperationLiteralChar*)operation.get());
	    break;
	case Operation::OP_LITERAL_STRING:
	    generate_operation_literal_string(tmp_values, mir_function, (OperationLiteralString*)operation.get());
	    break;
	case Operation::OP_LITERAL_INT:
	    generate_operation_literal_int(tmp_values, mir_function, (OperationLiteralInt*)operation.get());
	    break;
	case Operation::OP_LITERAL_FLOAT:
	    generate_operation_literal_float(tmp_values, mir_function, (OperationLiteralFloat*)operation.get());
	    break;
	case Operation::OP_POST_INCREMENT:
	    generate_operation_post_increment(tmp_values, mir_function, (OperationUnary*)operation.get());
	    break;
	case Operation::OP_POST_DECREMENT:
	    generate_operation_post_decrement(tmp_values, mir_function, (OperationUnary*)operation.get());
	    break;
	case Operation::OP_PRE_INCREMENT:
	    generate_operation_pre_increment(tmp_values, mir_function, (OperationUnary*)operation.get());
	    break;
	case Operation::OP_PRE_DECREMENT:
	    generate_operation_pre_decrement(tmp_values, mir_function, (OperationUnary*)operation.get());
	    break;
	case Operation::OP_ADD:
	    generate_operation_add(tmp_values, mir_function, (OperationBinary*)operation.get());
	    break;
	case Operation::OP_SUBTRACT:
	    generate_operation_subtract(tmp_values, mir_function, (OperationBinary*)operation.get());
	    break;
	case Operation::OP_MULTIPLY:
	    generate_operation_multiply(tmp_values, mir_function, (OperationBinary*)operation.get());
	    break;
	case Operation::OP_DIVIDE:
	    generate_operation_divide(tmp_values, mir_function, (OperationBinary*)operation.get());
	    break;
	case Operation::OP_ASSIGN:
	    generate_operation_assign(tmp_values, tmp_lvalues, mir_function, (OperationBinary*)operation.get());
	    break;
	case Operation::OP_JUMP_IF_EQUAL:
	    generate_operation_jump_if_equal(tmp_values, mir_function, (OperationJumpIfEqual*)operation.get());
	    break;
	case Operation::OP_JUMP:
	    generate_operation_jump(tmp_values, mir_function, (OperationJump*)operation.get());
	    break;
	case Operation::OP_RETURN:
	    return_value = generate_operation_return(tmp_values, mir_function, (OperationReturn*)operation.get());
	    break;
	}
	operation->dump();
    }
    return return_value;
}


void
CodeGeneratorLLVMContext::generate_function(const JLang::mir::Function & function)
{
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

    llvm::Value *return_value = nullptr;
    
    for (const auto blockid : function.get_blocks_in_order()) {
	// Create a new basic block to start insertion into.
	std::string block_name = std::string("BB") + std::to_string(blockid);
	llvm::BasicBlock *BB = llvm::BasicBlock::Create(*TheContext, block_name, TheFunction);
	Builder->CreateBr(BB);
	Builder->SetInsertPoint(BB);
	return_value = generate_basic_block(function, blockid);
    }

    if (return_value == nullptr) {
	Builder->CreateRet(
	    Builder->getInt32(0x1000)
	    );
    }
    else {
	fprintf(stderr, "Return already happened\n");
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

    TheModule->print(errs(), nullptr);
    
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
