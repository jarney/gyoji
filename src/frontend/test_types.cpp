#include <jlang-misc/input-source-file.hpp>
#include <jlang-frontend.hpp>
#include <jlang-frontend/type-resolver.hpp>
#include <jlang-mir.hpp>
#include <jlang-misc/test.hpp>

using namespace JLang::context;
using namespace JLang::frontend;
using namespace JLang::frontend::namespaces;
using namespace JLang::mir;


static
JLang::owned<MIR>
parse_to_mir(std::string & path, CompilerContext & context, std::string base_filename);

int main(int argc, char **argv)
{
    if (argc != 2) {
	fprintf(stderr, "First argument must be the path of the source directory\n");
	return -1;
    }
    printf("Testing types\n");
    
    std::string path(argv[1]);
    
    CompilerContext context;
    
    auto mir = std::move(parse_to_mir(path, context, "tests/type-resolution.j"));
    
    mir->get_types().dump(stderr);
    
    printf("    PASSED\n");
}

static
JLang::owned<MIR>
parse_to_mir(std::string & path, CompilerContext & context, std::string base_filename)
{
    std::string filename = path + std::string("/") + base_filename;
    
    int input = open(filename.c_str(), O_RDONLY);
    if (input == -1) {
	fprintf(stderr, "Cannot open file %s\n", base_filename.c_str());
	return nullptr;
    }
    
    JLang::misc::InputSourceFile input_source(input);
    auto mir =
	Parser::parse_to_mir(
	    context,
	    input_source
	    );
    close(input);
    return std::move(mir);
}
