#include <jlang-codegen.hpp>
#include "jlang-codegen-private.hpp"

using namespace llvm::sys;
using namespace JLang::codegen;

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
