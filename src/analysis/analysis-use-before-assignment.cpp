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

AnalysisPassUseBeforeAssignment::AnalysisPassUseBeforeAssignment(CompilerContext & _compiler_context)
    : AnalysisPass(_compiler_context, "use-before-initialization checks")
{}
AnalysisPassUseBeforeAssignment::~AnalysisPassUseBeforeAssignment()
{}

void
AnalysisPassUseBeforeAssignment::check(const Gyoji::mir::MIR & mir) const
{
    for (const auto & function : mir.get_functions().get_functions()) {
	check(*function);
    }
}

namespace Gyoji::analysis {
    class VariableUseVisitor : public OperationVisitor {
    public:
	VariableUseVisitor();
	~VariableUseVisitor();
	void visit(
	    size_t block_id,
	    const BasicBlock & block,
	    size_t operation_index,
	    const Operation & operation
	    );
	const std::vector<std::pair<size_t, size_t>> & get_loads();
    private:
	std::vector<std::pair<size_t, size_t>> variable_loads;
	std::vector<std::pair<size_t, size_t>> variable_stores;
    };
    
};

VariableUseVisitor::VariableUseVisitor()
    : OperationVisitor()
{
}

VariableUseVisitor::~VariableUseVisitor()
{}

const std::vector<std::pair<size_t, size_t>> &
VariableUseVisitor::get_loads()
{
    return variable_loads;
}

void VariableUseVisitor::visit(
    size_t block_id,
    const BasicBlock & block,
    size_t operation_index,
    const Operation & operation
    )    
{
    if (operation.get_type() == Operation::OP_LOCAL_VARIABLE) {
	variable_loads.push_back(std::pair(block_id, operation_index));
	// We have the variable name here, so we can track it directly.
    }
    else if (operation.get_type() == Operation::OP_ASSIGN) {
	variable_stores.push_back(std::pair(block_id, operation_index));
	// We don't have the variable here, so we need to trace it
	// back to the lvalue it's pointing to.
    }
}

void AnalysisPassUseBeforeAssignment::check(const Function & function) const
{
    // TODO:
    // For each 'use' operation, we need to
    // check whether there exists a path 'backward' up the chain of
    // basic blocks where an assignment took place.
    VariableUseVisitor visitor;
    
    function.iterate_operations(visitor);
//    fprintf(stderr, "Function %s\n", function.get_name().c_str());
//    for (const auto & load : visitor.get_loads()) {
//	fprintf(stderr, "Variable loads at %ld %ld\n", load.first, load.second);
//    }
    
}
