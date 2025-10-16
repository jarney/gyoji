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
    const auto & blocks = function.get_blocks();
    for (const auto & block_it : blocks) {
	const BasicBlock & block = *block_it.second;
	const std::vector<Gyoji::owned<Operation>> & operations = block.get_operations();

	bool terminated = false;
	for (const auto & operation_it : operations) {
	    const Operation & operation = *operation_it;
	    // We found an operation after we found a terminator.
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
		terminated = true;
		continue;
	    }
	}
    }
}
