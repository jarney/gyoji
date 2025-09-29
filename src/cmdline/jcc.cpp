#include <jlang-frontend.hpp>
#include <jlang-misc/input-source-file.hpp>
#include <jlang-analysis.hpp>
#include <jlang-codegen.hpp>

using namespace JLang::codegen;
using namespace JLang::context;
using namespace JLang::frontend;
using namespace JLang::mir;
using namespace JLang::analysis;

int main(int argc, char **argv)
{

    if (argc != 3) {
	fprintf(stderr, "Invalid number of arguments %d\n", argc);
	fprintf(stderr, "Usage: jcc source-file object-file\n");
	exit(1);
    }
    
    int input = open(argv[1], O_RDONLY);
    if (input == -1) {
	fprintf(stderr, "Cannot open file %s\n", argv[1]);
	exit(1);
    }
    
    CompilerContext context;
    JLang::misc::InputSourceFile input_source(input);
    
    JLang::owned<MIR> mir =
	Parser::parse_to_mir(
	    context,
	    input_source
	    );
    close(input);
    
    AnalysisPassTypeResolution type_resolution(context);
    type_resolution.check(mir->get_types());
    
    if (context.has_errors()) {
	context.get_errors().print();
	return -1;
    }
    
    std::string filename(argv[2]);
    generate_code(context, *mir, filename);
    
    return 0;
}
