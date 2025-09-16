#include <jlang-frontend/jsyntax.hpp>

#include <jlang.l.hpp>
#include <jlang.y.hpp>

using namespace JLang::frontend;
using namespace JLang::frontend::ast;
using namespace JLang::frontend::tree;
using namespace JLang::frontend::namespaces;
using namespace JLang::frontend::yacc;

Parser::Parser(NamespaceContext & _namespace_context)
  : yacc_context(_namespace_context)
{
}
Parser::~Parser()
{
}

int
Parser::parse(InputSource & _input_source)
{
  yyscan_t scanner;
  yylex_init(&scanner);
  //  yyset_in(input, scanner);

  LexContext lex_context(yacc_context.namespace_context, _input_source);
  yyset_extra(&lex_context, scanner);
  
  yacc::YaccParser parser { scanner, yacc_context };
  int rc = parser.parse();
  yylex_destroy(scanner);
  if (rc != 0) {
    printf("Syntax error\n");
    return rc;
  }
  return 0;
}

const TranslationUnit &
Parser::get_translation_unit() const
{
  return yacc_context.get_translation_unit();
}
const SyntaxNode &
Parser::get_syntax_node() const
{
  return yacc_context.get_translation_unit();
}

const NamespaceContext &
Parser::get_namespace_context() const
{
  return yacc_context.get_namespace_context();
}

