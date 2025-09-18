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

Parser::Parser(NamespaceContext_owned_ptr _namespace_context)
  : yacc_context(std::make_unique<ParseResult>(std::move(_namespace_context)))
{}

Parser::~Parser()
{
}

int
Parser::parse(InputSource & _input_source)
{
  yyscan_t scanner;
  yylex_init(&scanner);

  LexContext lex_context(
                         *yacc_context->namespace_context,
                         *yacc_context->token_stream,
                         _input_source);
  yyset_extra(&lex_context, scanner);

  
  yacc::YaccParser parser { scanner, *yacc_context };
  int rc = parser.parse();
  yylex_destroy(scanner);
  return rc;
}

ParseResult_owned_ptr
Parser::get_parse_result()
{
  return std::move(yacc_context);
}

