#include <gyoji-misc/input-source-file.hpp>
#include <gyoji-frontend.hpp>
#include <gyoji-frontend/type-resolver.hpp>
#include <gyoji-mir.hpp>
#include <gyoji-misc/test.hpp>

using namespace Gyoji::context;
using namespace Gyoji::frontend;
using namespace Gyoji::frontend::namespaces;
using namespace Gyoji::mir;


static
Gyoji::owned<MIR>
parse_to_mir(std::string & path, CompilerContext & context, std::string base_filename);

int main(int argc, char **argv)
{
    if (argc != 2) {
	fprintf(stderr, "First argument must be the path of the source directory\n");
	return -1;
    }
    printf("Testing types\n");
    
    std::string path(argv[1]);

    const char *filename = "tests/type-resolution.j";
    CompilerContext context(filename);
    
    auto mir = parse_to_mir(path, context, filename);
    
    mir->get_types().dump(stderr);
    
    printf("    PASSED\n");
}

static
Gyoji::owned<MIR>
parse_to_mir(std::string & path, CompilerContext & context, std::string base_filename)
{
    std::string filename = path + std::string("/") + base_filename;
    
    int input = open(filename.c_str(), O_RDONLY);
    if (input == -1) {
	fprintf(stderr, "Cannot open file %s\n", base_filename.c_str());
	return nullptr;
    }
    
    Gyoji::misc::InputSourceFile input_source(input);
    auto mir =
	Parser::parse_to_mir(
	    context,
	    input_source
	    );
    close(input);
    return mir;
}
