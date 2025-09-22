#include <jlang-codegen.hpp>
#include "jlang-codegen-private.hpp"

using namespace llvm::sys;
using namespace JLang::codegen;
using namespace JLang::mir;

CodeGeneratorLLVM::CodeGeneratorLLVM()
  : context(std::make_unique<CodeGeneratorLLVMContext>())
{}

CodeGeneratorLLVM::~CodeGeneratorLLVM()
{}


void
CodeGeneratorLLVM::initialize()
{
  // Open a new context and module.
  context->TheContext = std::make_unique<llvm::LLVMContext>();
  context->TheModule = std::make_unique<llvm::Module>("jlang LLVM Code Generator", *context->TheContext);
  // Create a new builder for the module.
  context->Builder = std::make_unique<llvm::IRBuilder<>>(*context->TheContext);

  //  register_type_builtins();
  //  register_operator_builtins();
}

llvm::Function *
CodeGeneratorLLVM::create_function(const JLang::mir::Function & function)
{
  // Make the function type:  double(double,double) etc.
  std::vector<llvm::Type *> llvm_arguments;
  const std::vector<FunctionArgument> & function_arguments = function.get_arguments();
  
  for (auto semantic_arg : function_arguments) {
    llvm::Type *atype = llvm::Type::getDoubleTy(*context->TheContext);
    llvm_arguments.push_back(atype);
  }

  llvm::Type* return_value_type = llvm::Type::getDoubleTy(*context->TheContext);
  
  llvm::FunctionType *FT =
    llvm::FunctionType::get(return_value_type, llvm_arguments, false);

  llvm::Function *F =
    llvm::Function::Create(FT, llvm::Function::ExternalLinkage, function.get_name(), context->TheModule.get());

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
llvm::AllocaInst *CodeGeneratorLLVM::CreateEntryBlockAlloca(
                                                            llvm::Function *TheFunction,
                                                            const llvm::StringRef & VarName
                                                            )
{
  llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                         TheFunction->getEntryBlock().begin()
                         );
  return TmpB.CreateAlloca(llvm::Type::getDoubleTy(*context->TheContext), nullptr, VarName);
}

void
CodeGeneratorLLVM::generate_function(const JLang::mir::Function & function)
{
  using namespace llvm;
  
  // Transfer ownership of the prototype to the FunctionProtos map, but keep a
  // reference to it for use below.
  llvm::Function *TheFunction = create_function(function);
  if (!TheFunction) {
    fprintf(stderr, "Function declaration not found\n");
  }

  // Create a new basic block to start insertion into.
  llvm::BasicBlock *BB = llvm::BasicBlock::Create(*context->TheContext, "entry", TheFunction);
  context->Builder->SetInsertPoint(BB);

  // Record the function arguments in the NamedValues map.
  //NamedValues.clear();
  for (auto &Arg : TheFunction->args()) {
    // Create an alloca for this variable.
    AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, Arg.getName());

    // Store the initial value into the alloca.
    context->Builder->CreateStore(&Arg, Alloca);

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
    llvm::Type *return_value_type = llvm::Type::getDoubleTy(*context->TheContext);
    context->Builder->CreateRet(
                       llvm::ConstantFP::get(*context->TheContext, llvm::APFloat(0.0))
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
CodeGeneratorLLVM::output(std::string filename)
{
  using namespace llvm;
  // Initialize the target registry etc.
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();

  auto TargetTriple = sys::getDefaultTargetTriple();
  context->TheModule->setTargetTriple(TargetTriple);

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

  context->TheModule->setDataLayout(TheTargetMachine->createDataLayout());

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

  pass.run(*context->TheModule);
  dest.flush();

  outs() << "Wrote " << filename << "\n";

  return 0;
}
