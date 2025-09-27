#include <jlang-codegen.hpp>
#include "jlang-codegen-private.hpp"

using namespace llvm::sys;
using namespace JLang::codegen;
using namespace JLang::mir;

CodeGeneratorLLVM::CodeGeneratorLLVM(const JLang::mir::MIR & _mir)
    : context(std::make_unique<CodeGeneratorLLVMContext>(_mir))
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
CodeGeneratorLLVMContext::CodeGeneratorLLVMContext(const JLang::mir::MIR & _mir)
    : mir(_mir)
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
    
    if (primitive->get_name() == "u8") {
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
CodeGeneratorLLVMContext::generate_basic_block(const JLang::mir::Function & mir_function, size_t blockid)
{
    fprintf(stderr, "Generating code for block BB%ld\n", blockid);
    const JLang::mir::BasicBlock & mir_block = mir_function.get_basic_block(blockid);

    for (const auto & operation : mir_block.get_operations()) {
	switch (operation->get_type()) {
	case Operation::OP_FUNCTION_CALL:
	    fprintf(stderr, "Generating a function call\n");
	    break;
	}
	operation->dump();
    }
    
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

    for (const auto blockid : function.get_blocks_in_order()) {
	// Create a new basic block to start insertion into.
	std::string block_name = std::string("BB") + std::to_string(blockid);
	llvm::BasicBlock *BB = llvm::BasicBlock::Create(*TheContext, block_name, TheFunction);
	Builder->SetInsertPoint(BB);
	generate_basic_block(function, blockid);
    }
    
    // Record the function arguments in the NamedValues map.
    //NamedValues.clear();
    for (auto &Arg : TheFunction->args()) {
	// Create an alloca for this variable.
	llvm::AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, Arg.getName());
	
	// Store the initial value into the alloca.
	Builder->CreateStore(&Arg, Alloca);
	
	// Add arguments to variable symbol table.
	//NamedValues[std::string(Arg.getName())] = Alloca;
    }
    
#if 0
    printf("Generating body\n");
    if (Value *RetVal = Body->codegen()) {
	printf("Generated body...\n");
	// Finish off the function.
	Builder->CreateRet(RetVal);
	
	// Validate the generated code, checking for consistency.
	verifyFunction(*TheFunction);
    }
#else
    
    //Value *block = codegen(*functiondef.scope_body);
    
#if 0
    if (Value *RetVal = llvm::ConstantFP::get(*TheContext, llvm::APFloat(0.0))) {
	printf("Generated body...\n");
	// Finish off the function.
	Builder->CreateRet(RetVal);
    }
#endif
    //  if (!block) {
    llvm::Type *return_value_type = llvm::Type::getDoubleTy(*TheContext);
    Builder->CreateRet(
	llvm::ConstantFP::get(*TheContext, llvm::APFloat(0.0))
	);
    //  }
    //  else {
    //    Builder->CreateRet(block);
    //  }
    
    // Validate the generated code, checking for consistency.
    verifyFunction(*TheFunction);
    
#endif
    
    //  for (auto &Arg : TheFunction->args()) {
    //    NamedValues.erase(std::string(Arg.getName()));
    //  }
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
