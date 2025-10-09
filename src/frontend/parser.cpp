#include <gyoji-frontend.hpp>

#include <gyoji.l.hpp>
#include <gyoji.y.hpp>

using namespace Gyoji::context;
using namespace Gyoji::mir;
using namespace Gyoji::frontend;
using namespace Gyoji::frontend::ast;
using namespace Gyoji::frontend::tree;
using namespace Gyoji::frontend::namespaces;
using namespace Gyoji::frontend::yacc;

Gyoji::owned<ParseResult>
Parser::parse(
    Gyoji::context::CompilerContext & _compiler_context,
    Gyoji::misc::InputSource & _input_source
    )
{
    auto namespace_context = std::make_unique<Gyoji::frontend::namespaces::NamespaceContext>();
    Gyoji::owned<ParseResult> result = std::make_unique<ParseResult>(
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
    parser.parse();
    yylex_destroy(scanner);
    
    return result;
}

Gyoji::owned<MIR>
Parser::parse_to_mir(
    Gyoji::context::CompilerContext & _compiler_context,
    Gyoji::misc::InputSource & _input_source
    )
{
    
    // We don't need to report an error at this point
    // because lack of a translation unit means
    // that our caller should not even have called us
    // and should report a syntax error at the
    // higher level.
    Gyoji::owned<ParseResult> parse_result = parse(_compiler_context, _input_source);
    Gyoji::owned<MIR> mir = std::make_unique<MIR>();
    
    if (!parse_result->has_translation_unit()) {
	// It's harmless to return an empty mir
	// to the next stages
	return mir;
    }

    Gyoji::mir::operation_static_init();

    fprintf(stderr, "============================\n");
    fprintf(stderr, "Type and symbol table resolution pass\n");
    fprintf(stderr, "============================\n");
    // First, resolve all of the type definitions.
    // Also at this stage, we resolve the function declarations.
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
    
    return mir;
}
