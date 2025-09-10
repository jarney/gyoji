#include "jbackend-llvm.hpp"

using namespace llvm::sys;
using namespace JSemantics;
using namespace JLang::Backend::LLVM;

LLVMType::LLVMType(llvm::LLVMContext & _context)
  : context(_context)
{}
LLVMType::~LLVMType()
{}

LLVMTypeVoid::LLVMTypeVoid(llvm::LLVMContext & _context)
  : LLVMType(_context)
{}
LLVMTypeVoid::~LLVMTypeVoid()
{}
llvm::Type *LLVMTypeVoid::get_type()
{
  return llvm::Type::getVoidTy(context);
}
llvm::Value *LLVMTypeVoid::get_initializer_default()
{
  return llvm::UndefValue::get(llvm::Type::getVoidTy(context));
}

LLVMTypeDouble::LLVMTypeDouble(llvm::LLVMContext & _context)
  : LLVMType(_context)
{}
LLVMTypeDouble::~LLVMTypeDouble()
{}
llvm::Type *LLVMTypeDouble::get_type()
{
  return llvm::Type::getDoubleTy(context);
}
llvm::Value *LLVMTypeDouble::get_initializer_default()
{
  return llvm::ConstantFP::get(context, llvm::APFloat(0.0));
}

LLVMTranslationUnitVisitor::LLVMTranslationUnitVisitor()
{}
LLVMTranslationUnitVisitor::~LLVMTranslationUnitVisitor()
{}

void
LLVMTranslationUnitVisitor::visit(GlobalVariableDefinition &globaldef)
{
  std::string &name = globaldef.name;
  
  TheModule->getOrInsertGlobal(name, llvm::Type::getDoubleTy(*TheContext));
  llvm::GlobalVariable *globalVar = TheModule->getNamedGlobal(name);
  llvm::Constant *initValue = llvm::ConstantFP::get(*TheContext, llvm::APFloat(0.0));
  globalVar->setInitializer(initValue);
  printf("Storing global variable %s\n", name.c_str());
  NamedValues[name] = globalVar;
}

llvm::Function *LLVMTranslationUnitVisitor::getFunction(std::string name)
{
  // First, see if the function has already been added to the current module.
  if (auto *F = TheModule->getFunction(name))
    return F;

  // If not, check whether we can codegen the declaration from some existing
  // prototype.
  auto it = FunctionProtos.find(name);
  if (it != FunctionProtos.end()) {
    visit(*it->second);
    return TheModule->getFunction(name);
  }

  // If no existing prototype exists, return null.
  return nullptr;
}

void
LLVMTranslationUnitVisitor::visit(FunctionDeclaration &functiondecl)
{
  // Make the function type:  double(double,double) etc.
  std::vector<llvm::Type *> Doubles(functiondecl.arg_type.size(), llvm::Type::getDoubleTy(*TheContext));
  //  llvm::FunctionType *FT =
  //    llvm::FunctionType::get(llvm::Type::getDoubleTy(*TheContext), Doubles, false);

  LLVMType::ptr return_value_type = types[functiondecl.return_type];
  
  llvm::FunctionType *FT =
    llvm::FunctionType::get(return_value_type->get_type(), Doubles, false);

  llvm::Function *F =
    llvm::Function::Create(FT, llvm::Function::ExternalLinkage, functiondecl.name, TheModule.get());

  // Set names for all arguments.
  unsigned Idx = 0;
  for (auto &Arg : F->args())
    Arg.setName(functiondecl.arg_type[Idx++]);
}

/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
/// the function.  This is used for mutable variables etc.
llvm::AllocaInst *LLVMTranslationUnitVisitor::CreateEntryBlockAlloca(llvm::Function *TheFunction,
                                                llvm::StringRef VarName) {
  llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                         TheFunction->getEntryBlock().begin()
                         );
  return TmpB.CreateAlloca(llvm::Type::getDoubleTy(*TheContext), nullptr, VarName);
}

void
LLVMTranslationUnitVisitor::visit(FunctionDefinition &functiondef)
{
  using namespace llvm;
  
  // Transfer ownership of the prototype to the FunctionProtos map, but keep a
  // reference to it for use below.
  auto functionDeclaration = functiondef.functionDeclaration;
  printf("Processing function %s\n", functionDeclaration->name.c_str());
  auto &P = *functionDeclaration;
  FunctionProtos[functionDeclaration->name] = functionDeclaration;
  Function *TheFunction = getFunction(functionDeclaration->name);
  if (!TheFunction) {
    fprintf(stderr, "Function declaration not found\n");
  }

  // Create a new basic block to start insertion into.
  BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
  Builder->SetInsertPoint(BB);

  // Record the function arguments in the NamedValues map.
  //NamedValues.clear();
  for (auto &Arg : TheFunction->args()) {
    // Create an alloca for this variable.
    AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, Arg.getName());

    // Store the initial value into the alloca.
    Builder->CreateStore(&Arg, Alloca);

    // Add arguments to variable symbol table.
    NamedValues[std::string(Arg.getName())] = Alloca;
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

  //LLVMScopeBodyVisitor scope_body_visitor;
  //scope_body.visit(scope_body_visitor);
  
#if 0
  if (Value *RetVal = llvm::ConstantFP::get(*TheContext, llvm::APFloat(0.0))) {
    printf("Generated body...\n");
    // Finish off the function.
    Builder->CreateRet(RetVal);
  }
#endif
  
  LLVMType::ptr return_value_type = types[functionDeclaration->return_type];
  Builder->CreateRet(return_value_type->get_initializer_default());
  
  // Validate the generated code, checking for consistency.
  verifyFunction(*TheFunction);

#endif
  
  for (auto &Arg : TheFunction->args()) {
    NamedValues.erase(std::string(Arg.getName()));
  }
}

void
LLVMTranslationUnitVisitor::visit(TranslationUnit &translation_unit)
{
  printf("Processing types\n");
  // Here, we process the types, making sure
  // that each type is resolved to a memory layout,
  // access, and namespace so that they can be
  // referred to later when we need to start
  // instantiating things.
  for (auto ty : translation_unit.types) {
  }

  printf("Processing globals\n");
  for (auto gl : translation_unit.globals) {
    visit(*gl);
  }
  printf("Processing functions\n");
  for (auto fn : translation_unit.function_definitions) {
    visit(*fn);
  }
}

void
LLVMTranslationUnitVisitor::initialize()
{
  // Open a new context and module.
  TheContext = std::make_unique<llvm::LLVMContext>();
  TheModule = std::make_unique<llvm::Module>("jlang LLVM Code Generator", *TheContext);

  // Create a new builder for the module.
  Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);

  types["void"] = std::make_shared<LLVMTypeVoid>(*TheContext);
  types["double"] = std::make_shared<LLVMTypeDouble>(*TheContext);
}

int
LLVMTranslationUnitVisitor::output(std::string filename)
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
