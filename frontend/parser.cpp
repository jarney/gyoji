#include <jlang-frontend/frontend.hpp>

#include <jlang.l.hpp>
#include <jlang.y.hpp>

using namespace JLang::frontend;
using namespace JLang::frontend::ast;
using namespace JLang::frontend::tree;
using namespace JLang::frontend::namespaces;
using namespace JLang::frontend::yacc;

Parser::Parser(NamespaceContext & _namespace_context)
{
  yacc_context = std::make_unique<ParseResult>(_namespace_context);
}
Parser::~Parser()
{
}

int
Parser::parse(InputSource & _input_source)
{
  yyscan_t scanner;
  yylex_init(&scanner);

  LexContext lex_context(
                         yacc_context->namespace_context,
                         *yacc_context->token_stream,
                         _input_source);
  yyset_extra(&lex_context, scanner);

  
  yacc::YaccParser parser { scanner, *yacc_context };
  int rc = parser.parse();
  yylex_destroy(scanner);
  if (rc != 0) {
    printf("Syntax error\n");
    return rc;
  }
  return 0;
}

ParseResult_owned_ptr
Parser::get_parse_result()
{
  return std::move(yacc_context);
}

