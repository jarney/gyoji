#include <jlang-frontend.hpp>
#define _JLANG_INTERNAL
#include <lex-context.hpp>
#undef _JLANG_INTERNAL

#include <jlang.l.hpp>
#include <jlang.y.hpp>

using namespace JLang::context;
using namespace JLang::mir;
using namespace JLang::frontend;
using namespace JLang::frontend::ast;
using namespace JLang::frontend::tree;
using namespace JLang::frontend::namespaces;
using namespace JLang::frontend::yacc;

JLang::owned<ParseResult>
Parser::parse(
              JLang::context::CompilerContext & _compiler_context,
              JLang::misc::InputSource & _input_source
              )
{
  auto namespace_context = std::make_unique<JLang::frontend::namespaces::NamespaceContext>();
  JLang::owned<ParseResult> result = std::make_unique<ParseResult>(
                                                                   _compiler_context,
                                                                   std::move(namespace_context)
                                                                   );
  
  yyscan_t scanner;
  yylex_init(&scanner);

  LexContext lex_context(
                         *result->namespace_context,
                         _compiler_context,
                         _input_source);
  yyset_extra(&lex_context, scanner);
  
  yacc::YaccParser parser { scanner, *result };
  int rc = parser.parse();
  yylex_destroy(scanner);
  
  return std::move(result);
}

JLang::owned<MIR>
Parser::parse_to_mir(
                     JLang::context::CompilerContext & _compiler_context,
                     JLang::misc::InputSource & _input_source
                     )
{
  JLang::owned<MIR> mir = std::make_unique<MIR>();


  JLang::owned<ParseResult> parse_result = parse(_compiler_context, _input_source);

  // Lowering for types.
  resolve_types(mir->get_types(), *parse_result);

  // Lowering for functions (not done yet)
  //resolve_functions(mir->get_functions(), *parse_result);

  return std::move(mir);
}
