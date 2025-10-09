#include <gyoji-frontend.hpp>
#include <gyoji-misc/input-source-file.hpp>
#include <gyoji-analysis.hpp>
#include <gyoji-codegen.hpp>

using namespace Gyoji::codegen;
using namespace Gyoji::context;
using namespace Gyoji::frontend;
using namespace Gyoji::mir;
using namespace Gyoji::analysis;

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
    std::string input_filename(argv[1]);
    std::string output_filename(argv[2]);
    
    CompilerContext context(input_filename);
    Gyoji::misc::InputSourceFile input_source(input);
    
    Gyoji::owned<MIR> mir =
	Parser::parse_to_mir(
	    context,
	    input_source
	    );
    close(input);

    // Dump our MIR
    // for debugging/review purposes
    // before any analysis or code-generation
    // passes.
    {
	std::string mir_filename = output_filename + std::string(".mir");
	FILE *mir_output = fopen(mir_filename.c_str(), "w");
	mir->dump(mir_output);
	fclose(mir_output);
    }

    // If we had errors at the MIR construction
    // phase, it is likely we'll have an unsuitable
    // MIR for analysis, so don't bother.
    if (context.has_errors()) {
	context.get_errors().print();
	return -1;
    }

    // Make sure that all types that are used in functions
    // actually have 'complete' definitions.
    std::vector<Gyoji::owned<AnalysisPass>> analysis_passes;
    
    analysis_passes.push_back(std::make_unique<AnalysisPassTypeResolution>(context));
    analysis_passes.push_back(std::make_unique<AnalysisPassUnreachable>(context));
    analysis_passes.push_back(std::make_unique<AnalysisPassBorrowChecker>(context));

    
    for (const auto & analysis_pass : analysis_passes) {
	fprintf(stderr, "============================\n");
	fprintf(stderr, "Analysis pass %s\n", analysis_pass->get_name().c_str());
	fprintf(stderr, "============================\n");
	analysis_pass->check(*mir);
    }

    if (context.has_errors()) {
	context.get_errors().print();
	return -1;
    }

    fprintf(stderr, "============================\n");
    fprintf(stderr, "Code Generation Pass\n");
    fprintf(stderr, "============================\n");
    // This leaks memory. The code-generation
    // stage is a bit problematic
    // because we're not really cleaning up the
    // LLVM stuff at the moment.
    generate_code(context, *mir, output_filename);
    
    if (context.has_errors()) {
	context.get_errors().print();
	return -1;
    }
    return 0;
}
