#include "jbackend-llvm.hpp"
#include "jbackend-llvm-types.hpp"
#include "jbackend-llvm-operators.hpp"

using namespace llvm::sys;
using namespace JSemantics;
using namespace JLang::Backend::LLVM;

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
LLVMTranslationUnitVisitor::visit(ScopeBlock &scope_block)
{
}

void
LLVMTranslationUnitVisitor::visit(FunctionDeclaration &functiondecl)
{
  // Make the function type:  double(double,double) etc.
  std::vector<llvm::Type *> Doubles;
  for (auto pair : functiondecl.arg_types) {
    LLVMType::ptr atype = types[pair.first];
    fprintf(stderr, "Adding type %s\n", pair.first.c_str());
    Doubles.push_back(atype->get_type());
  }
  fprintf(stderr, "Doubles type is %ld\n", Doubles.size());
  //  llvm::FunctionType *FT =
  //    llvm::FunctionType::get(llvm::Type::getDoubleTy(*TheContext), Doubles, false);

  LLVMType::ptr return_value_type = types[functiondecl.return_type];
  
  llvm::FunctionType *FT =
    llvm::FunctionType::get(return_value_type->get_type(), Doubles, false);

  llvm::Function *F =
    llvm::Function::Create(FT, llvm::Function::ExternalLinkage, functiondecl.name, TheModule.get());

  // Set names for all arguments.
  unsigned Idx = 0;
  for (auto &Arg : F->args()) {
    fprintf(stderr, "Indirecting arg_types\n");
    auto pair = functiondecl.arg_types[Idx++];
    fprintf(stderr, "Adding type %s %s\n", pair.first.c_str(), pair.second.c_str());
    Arg.setName(pair.second);
  }
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
  auto function_declaration = functiondef.function_declaration;
  printf("Processing function %s\n", function_declaration->name.c_str());
  auto &P = *function_declaration;
  FunctionProtos[function_declaration->name] = function_declaration;
  Function *TheFunction = getFunction(function_declaration->name);
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

  visit(*functiondef.scope_block);
  
#if 0
  if (Value *RetVal = llvm::ConstantFP::get(*TheContext, llvm::APFloat(0.0))) {
    printf("Generated body...\n");
    // Finish off the function.
    Builder->CreateRet(RetVal);
  }
#endif
  
  LLVMType::ptr return_value_type = types[function_declaration->return_type];
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

  register_type_builtins();
  register_operator_builtins();
}

void
LLVMTranslationUnitVisitor::register_type_builtins()
{
  // Register the built-in types
  types["void"] = std::make_shared<LLVMTypeVoid>(*TheContext);
  types["u8"] = std::make_shared<LLVMTypeDouble>(*TheContext);
  types["i16"] = std::make_shared<LLVMTypeDouble>(*TheContext);
  types["i32"] = std::make_shared<LLVMTypeDouble>(*TheContext);
  types["i64"] = std::make_shared<LLVMTypeDouble>(*TheContext);

  types["u16"] = std::make_shared<LLVMTypeDouble>(*TheContext);
  types["u32"] = std::make_shared<LLVMTypeDouble>(*TheContext);
  types["u64"] = std::make_shared<LLVMTypeDouble>(*TheContext);

  types["f32"] = std::make_shared<LLVMTypeDouble>(*TheContext);
  types["f64"] = std::make_shared<LLVMTypeDouble>(*TheContext);
}


void
LLVMTranslationUnitVisitor::register_operator_builtins()
{
  // Operator builtins are the basic operations
  // like assignment, addition, etc.
  // Each of them has rules about what it can operate on
  // and what it can produce.  Casting is not allowed
  // in the language.  If you need casting, you need to
  // provide a 'cast' function that can change types
  // in a library or something.
  
  binary_operators.register_operator("+", "double", "double", std::make_shared<LLVMBinaryOperatorAddDouble>(*Builder));
  binary_operators.register_operator("+", "int", "int", std::make_shared<LLVMBinaryOperatorAddDouble>(*Builder));
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
