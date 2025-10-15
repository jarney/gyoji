#include "jformat-identity.hpp"
#include <gyoji-misc/input-source-file.hpp>

using namespace Gyoji::context;
using namespace Gyoji::frontend;
using namespace Gyoji::frontend::tree;
using namespace Gyoji::frontend::namespaces;
using namespace Gyoji::cmdline;

int main(int argc, char **argv)
{
    
    if (argc != 2) {
	fprintf(stderr, "Invalid number of arguments %d\n", argc);
	fprintf(stderr, "Usage: parser backend file\n");
	exit(1);
    }
    
    int input = open(argv[1], O_RDONLY);
    if (input == -1) {
	fprintf(stderr, "Cannot open file %s\n", argv[1]);
	exit(1);
    }
    
    CompilerContext context(argv[1]);
    
    Gyoji::misc::InputSourceFile input_source(input);
    
    Gyoji::owned<ParseResult> parse_result = 
        Parser::parse(
	    context,
	    input_source
	    );
    close(input);
    if (parse_result->has_errors()) {
	parse_result->get_errors().print();
	return -1;
    }
    
    parse_result->get_ns2_context().dump();
    
    return 0;
}
