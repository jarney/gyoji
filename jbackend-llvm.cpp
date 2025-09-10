#include "jbackend-llvm.hpp"
#include "jsemantics.hpp"

using namespace llvm;
using namespace llvm::sys;
using namespace JSemantics;

JBackendLLVM::JBackendLLVM()
{
}

JBackendLLVM::~JBackendLLVM()
{}

class LLVMTranslationUnitVisitor : public Visitor<TranslationUnit> {
public:
  LLVMTranslationUnitVisitor();
  ~LLVMTranslationUnitVisitor();
  void visit(TranslationUnit &visitable);
};

LLVMTranslationUnitVisitor::LLVMTranslationUnitVisitor()
{}
LLVMTranslationUnitVisitor::~LLVMTranslationUnitVisitor()
{}

void
LLVMTranslationUnitVisitor::visit(TranslationUnit &translation_unit)
{
  printf("Processing translation unit\n");
}

/**
 * This is pretty early in the processing of a compilation.  The input
 * here is an abstract syntax tree node (ASTNode) which represents
 * the raw parsed input (warts and all).  The only thing that we
 * really can rely on here is that the syntax conforms to the
 * BNF of the language.
 *
 * Processing goes in several stages:
 *   * Convert syntax tree into a semantic tree.  The semantic tree
 *     throws away much of the input (whitespace, comments, etc)
 *     and formats it into a tree that is specialized to contain
 *     the semantic meanings of things (for example a variable or
 *     an expression).
 *
 *  * The next step is to identify the types (TODO) and take the requested
 *    types and composite types and assemble them into things that
 *    represent the heirarchy.  This type information is required
 *    by the borrow checker to, for example, make sure that we
 *    invalidate a class borrow when one of the members is touched.
 *
 *  * Once this is done, the "borrow checker" (TODO) can start to
 *    analyze the semantics and check that it is 'semantically valid'
 *    according to the borrow rules.  We will be using mainly the
 *    Polonius technique to evaluate borrow rules.
 *
 *  * Finally, the 
 */
int JBackendLLVM::process(ASTNode::ptr ast_translation_unit)
{
  TranslationUnit::ptr translation_unit = from_ast(ast_translation_unit);

  // TODO: Resolve the types into fully-qualified namespaces.

  // TODO: Run the borrow checker to make sure that the requested
  // semantics are adhered to.

  // Finally, perform the LLVM code generation
  // the code generation should be factored out into a separate step.
  
  // Open a new context and module.
  TheContext = std::make_unique<LLVMContext>();
  TheModule = std::make_unique<Module>("my cool jit", *TheContext);

  // Create a new builder for the module.
  Builder = std::make_unique<IRBuilder<>>(*TheContext);

  LLVMTranslationUnitVisitor llvm_visitor;
  
  translation_unit->visit(llvm_visitor);

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

  auto Filename = "output.o";
  std::error_code EC;
  raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);

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

  outs() << "Wrote " << Filename << "\n";

  return 0;
}
