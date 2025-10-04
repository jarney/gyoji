#include <jlang-misc/input-source-file.hpp>
#include <jlang-frontend.hpp>
#include <jlang-context.hpp>
#include <jlang.l.hpp>
#include <jlang.y.hpp>

using namespace JLang::context;
using namespace JLang::frontend::tree;
using namespace JLang::frontend::yacc;

int main(int argc, char **argv)
{
    if (argc != 2) {
	fprintf(stderr, "Invalid number of arguments %d\n", argc);
	fprintf(stderr, "Usage: jformat-identity file\n");
	exit(1);
    }
    
    int input = open(argv[1], O_RDONLY);
    if (input == -1) {
	fprintf(stderr, "Cannot open file %s\n", argv[1]);
	exit(1);
    }
    
    CompilerContext context(argv[1]);
    JLang::frontend::namespaces::NamespaceContext namespace_context;
    
    JLang::misc::InputSourceFile input_source(input);
    
    LexContext lex_context(
	namespace_context,
	context,
	input_source);
    
    yyscan_t scanner;
    yylex_init(&scanner);
    yyset_extra(&lex_context, scanner);
    
    while (true) {
	JLang::frontend::yacc::YaccParser::semantic_type lvalue;
	int rc = yylex (&lvalue, scanner);
	if (rc == 0) {
	    break;
	}
	const JLang::owned<JLang::frontend::tree::Terminal> & token = lvalue.as<JLang::owned<JLang::frontend::tree::Terminal>>();
	printf("%ld %ld : %s %s\n",
	       token->get_source_ref().get_line(),
	       token->get_source_ref().get_column(),
	       token->get_type().c_str(),
	       token->get_value().c_str());
    }
    
    return 0;
}
