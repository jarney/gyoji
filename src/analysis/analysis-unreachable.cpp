#include <jlang-analysis.hpp>
#include <stdio.h>

using namespace JLang::mir;
using namespace JLang::context;
using namespace JLang::analysis;

AnalysisPassUnreachable::AnalysisPassUnreachable(CompilerContext & _compiler_context)
    : AnalysisPass(_compiler_context)
{}
AnalysisPassUnreachable::~AnalysisPassUnreachable()
{}

void
AnalysisPassUnreachable::check(const MIR & mir) const
{
    for (const auto & function : mir.get_functions().get_functions()) {
	check(*function);
    }
}

void AnalysisPassUnreachable::check(const Function & function) const
{
    const auto & blocks = function.get_blocks();
    for (const auto & block_it : blocks) {
	const BasicBlock & block = *block_it.second;
	const std::vector<JLang::owned<Operation>> & operations = block.get_operations();

	bool terminated = false;
	for (const auto & operation_it : operations) {
	    const Operation & operation = *operation_it;
	    // We found an operation after we found a terminator.
	    fprintf(stderr, "Found terminating operator %s\n", operation.get_description().c_str());
	    if (terminated) {
		get_compiler_context()
		    .get_errors()
		    .add_simple_error(operation.get_source_ref(),
				      "Unreachable Statement",
				      "Unreachable statement"
			);
		break;
	    }
	    // We found a terminating operation.
	    if (operation.is_terminating()) {
		fprintf(stderr, "Terminating\n");
		terminated = true;
		continue;
	    }
	}
    }
}
