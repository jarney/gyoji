/* Copyright 2025 Jonathan S. Arney
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      https://github.com/jarney/gyoji/blob/master/LICENSE
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <gyoji-analysis.hpp>
#include <stdio.h>

using namespace Gyoji::mir;
using namespace Gyoji::context;
using namespace Gyoji::analysis;

AnalysisPassUnreachable::AnalysisPassUnreachable(CompilerContext & _compiler_context)
    : AnalysisPass(_compiler_context, "unreachable analysis")
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
    // Note that we do assume that all empty unreachable
    // blocks have already been culled.
    
    // Check for operations after the terminator.
    // These are by definition unreachable because
    // the terminator stops execution, so nothing after
    // that will be reachable.
    const auto & blocks = function.get_blocks();
    for (const auto & block_it : blocks) {
	const BasicBlock & block = *block_it.second;
	const std::vector<Gyoji::owned<Operation>> & operations = block.get_operations();
	if (block.get_reachable_from().size() == 0 && block_it.first != 0) {
	    // Unreachable if the block is unreachable and has anything at all inside it.
	    if (operations.size() != 0) {
		const auto & op = operations.at(operations.size()-1);
		get_compiler_context()
		    .get_errors()
		    .add_simple_error(op->get_source_ref(),
				      "Unreachable statement",
				      std::string("Statement is unreachable.")
			);
	    }
	}
	else {
	    // Unreachable if the block is reachable and
	    // there are statements after the return.
	    bool terminated = false;
	    for (const auto & op : operations) {
		const auto & operation = *op;
		if (terminated) {
		    get_compiler_context()
			.get_errors()
			.add_simple_error(operation.get_source_ref(),
					  "Unreachable Statement",
					  std::string("Function ")
					  + function.get_name()
					  + std::string(" contains unreachable statement.")
			    );
		    break;
		}
		// We found a terminating operation.
		if (operation.is_terminating()) {
		    terminated = true;
		    continue;
		}
	    }
	}
    }
    
    // First, check that all blocks are connected
    // in a reachability graph by doing a depth-first search
    // on blocks.
//    check_all_blocks_reachable(function);


}
