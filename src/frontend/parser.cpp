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
    
    // We don't need to report an error at this point
    // because lack of a translation unit means
    // that our caller should not even have called us
    // and should report a syntax error at the
    // higher level.
    JLang::owned<ParseResult> parse_result = parse(_compiler_context, _input_source);
    JLang::owned<MIR> mir = std::make_unique<MIR>();
    
    if (!parse_result->has_translation_unit()) {
	// It's harmless to return an empty mir
	// to the next stages
	return mir;
    }

    fprintf(stderr, "============================\n");
    fprintf(stderr, "Symbol table resolution pass\n");
    fprintf(stderr, "============================\n");
    // First, resolve all of the type definitions.
    // Also at this stage, we resolve the function declarations
    TypeResolver type_resolver(_compiler_context,
			       parse_result->get_translation_unit(),
			       *mir);
    type_resolver.resolve();

    fprintf(stderr, "============================\n");
    fprintf(stderr, "Function resolution pass\n");
    fprintf(stderr, "============================\n");
    FunctionResolver function_resolver(_compiler_context,
				       *parse_result,
				       *mir,
				       type_resolver);
    function_resolver.resolve();
    
    return std::move(mir);
}
