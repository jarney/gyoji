#include <jlang-frontend/jsyntax.hpp>
#include "test_syntax_tree.hpp"
#include <jlang-misc/test.hpp>

using namespace JLang::frontend::alt_imp;

int main(int argc, char **argv)
{
  printf("Testing namespace search functionality\n");

  FileStatementList::owned_ptr fsl = std::make_unique<FileStatementList>();
  Terminal::owned_ptr t = std::make_unique<Terminal>();
  t->value = std::string("foo");
  t->lineno = 12;
  t->fully_qualified_name = std::string("::eof");
  t->non_syntax.push_back(std::make_unique<TerminalNonSyntax>(TerminalNonSyntaxType::EXTRA_COMMENT_SINGLE_LINE, "SomeComment"));
  
  TranslationUnit::owned_ptr tu = std::make_unique<TranslationUnit>(std::move(fsl), std::move(t));

  JBackendFormatTree tree;
  tree.process(tu.get());
  
  printf("    PASSED\n");

}
