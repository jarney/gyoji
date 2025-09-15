#include <jlang-codegen/jbackend-llvm.hpp>
#include <jlang-codegen/jsemantics.hpp>

using namespace llvm;
using namespace llvm::sys;
using namespace JLang::codegen::semantics;

using namespace JLang::frontend;
using namespace JLang::Backend::LLVM;

JBackendLLVM::JBackendLLVM()
{
}

JBackendLLVM::~JBackendLLVM()
{}

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
  int rc = 0;
  TranslationUnit::ptr translation_unit = from_ast(ast_translation_unit);
  
  // TODO: Resolve the types into fully-qualified namespaces.

  // TODO: Check semantic rules that cannot be evaluated during parse
  //         * Function named arguments may not be void.
  //         * Type consistency (function return values and expression types)
  //         * Functions have return statements that match function return values.
  //         * Run the borrow checker to make sure that the requested
  //           semantics are adhered to.

  // Finally, perform the LLVM code generation
  // the code generation should be factored out into a separate step.
  
  LLVMTranslationUnitVisitor llvm_visitor;
  llvm_visitor.initialize();
  
  translation_unit->visit(llvm_visitor);
  
  rc = llvm_visitor.output("output.o");

  return rc;
}
