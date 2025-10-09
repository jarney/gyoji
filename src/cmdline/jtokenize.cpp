#include <gyoji-misc/input-source-file.hpp>
#include <gyoji-frontend.hpp>
#include <gyoji-context.hpp>
#include <gyoji.l.hpp>
#include <gyoji.y.hpp>

using namespace Gyoji::context;
using namespace Gyoji::frontend::tree;
using namespace Gyoji::frontend::yacc;

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
    Gyoji::frontend::namespaces::NamespaceContext namespace_context;
    
    Gyoji::misc::InputSourceFile input_source(input);
    
    LexContext lex_context(
	namespace_context,
	context,
	input_source);
    
    yyscan_t scanner;
    yylex_init(&scanner);
    yyset_extra(&lex_context, scanner);
    
    while (true) {
	Gyoji::frontend::yacc::YaccParser::semantic_type lvalue;
	int rc = yylex (&lvalue, scanner);
	if (rc == 0) {
	    break;
	}
	const Gyoji::owned<Gyoji::frontend::tree::Terminal> & token = lvalue.as<Gyoji::owned<Gyoji::frontend::tree::Terminal>>();
	printf("%ld %ld : %d %s\n",
	       token->get_source_ref().get_line(),
	       token->get_source_ref().get_column(),
	       token->get_type(),
	       token->get_value().c_str());
    }
    
    return 0;
}
