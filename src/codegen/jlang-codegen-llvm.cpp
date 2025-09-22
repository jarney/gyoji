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
{ context->initialize(); }

void
CodeGeneratorLLVM::generate(const MIR & _mir)
{ context->generate(_mir); }

int
CodeGeneratorLLVM::output(std::string filename)
{ return context->output(filename); }
/////////////////////////////////////
// CodeGeneratorLLVMContext
/////////////////////////////////////

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
  
  for (auto semantic_arg : function_arguments) {
    llvm::Type *atype = llvm::Type::getDoubleTy(*TheContext);
    llvm_arguments.push_back(atype);
  }

  llvm::Type* return_value_type = llvm::Type::getDoubleTy(*TheContext);
  
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

void
CodeGeneratorLLVMContext::create_types(const MIR & _mir)
{

}


void
CodeGeneratorLLVMContext::generate(const MIR & _mir)
{
  create_types(_mir);
  
  const Functions & functions = _mir.get_functions();
  for (auto const & function : functions.get_functions()) {
    fprintf(stderr, "Generating for function %s\n", function->get_name().c_str());
    generate_function(*function);
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

  // Create a new basic block to start insertion into.
  llvm::BasicBlock *BB = llvm::BasicBlock::Create(*TheContext, "entry", TheFunction);
  Builder->SetInsertPoint(BB);

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
CodeGeneratorLLVMContext::output(std::string filename)
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
