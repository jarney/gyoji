#include <jlang-frontend.hpp>
#define _JLANG_INTERNAL
#include <lex-context.hpp>
#undef _JLANG_INTERNAL

#include <jlang.l.hpp>
#include <jlang.y.hpp>

using namespace JLang::errors;
using namespace JLang::frontend;
using namespace JLang::frontend::ast;
using namespace JLang::frontend::tree;
using namespace JLang::frontend::namespaces;
using namespace JLang::frontend::yacc;

::JLang::owned<ParseResult>
Parser::parse(::JLang::owned<NamespaceContext> _namespace_context, InputSource & _input_source)
{
  ::JLang::owned<ParseResult> result = std::make_unique<ParseResult>(std::move(_namespace_context));
  
  yyscan_t scanner;
  yylex_init(&scanner);

  LexContext lex_context(
                         *result->namespace_context,
                         *result->token_stream,
                         _input_source);
  yyset_extra(&lex_context, scanner);
  
  yacc::YaccParser parser { scanner, *result };
  int rc = parser.parse();
  yylex_destroy(scanner);
  
  return std::move(result);
}
