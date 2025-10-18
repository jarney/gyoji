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

AnalysisPassReturnValues::AnalysisPassReturnValues(CompilerContext & _compiler_context)
    : AnalysisPass(_compiler_context, "return-value consistency analysis")
{}
AnalysisPassReturnValues::~AnalysisPassReturnValues()
{}

void
AnalysisPassReturnValues::check(const Gyoji::mir::MIR & mir) const
{
    for (const auto & function : mir.get_functions().get_functions()) {
	check(*function);
    }
}

namespace Gyoji::analysis {
    class ReturnOperationVisitor : public OperationVisitor {
    public:
	ReturnOperationVisitor(
	    CompilerContext & _compiler_context,
	    const Function & _function
	    );
	~ReturnOperationVisitor();

	void visit(
	    size_t block_id,
	    const BasicBlock & block,
	    size_t operation_index,
	    const Operation & operation
	    );
    private:
	CompilerContext & compiler_context;
	const Function & function;
	const Type *return_type;
    };
    
};

ReturnOperationVisitor::ReturnOperationVisitor(
    CompilerContext & _compiler_context,
    const Function & _function
    )
    : OperationVisitor()
    , compiler_context(_compiler_context)
    , function(_function)
    , return_type(_function.get_return_type())
{
}

ReturnOperationVisitor::~ReturnOperationVisitor()
{}

void ReturnOperationVisitor::visit(
    size_t block_id,
    const BasicBlock & block,
    size_t operation_index,
    const Operation & operation
    )    
{
    if (operation.get_type() == Operation::OP_RETURN) {
	const Type *operation_type = function.tmpvar_get(operation.get_operands().at(0));
	if (return_type->get_name() != operation_type->get_name()) {
	    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Return statement returns incorrec type.");
	    error->add_message(
		operation.get_source_ref(),
		std::string("Return statement returns value of type ") + operation_type->get_name() + std::string(" but function is declared to return ") + return_type->get_name()
		);
	    error->add_message(
		function.get_source_ref(),
		"Return-value of function declared here."
		);
	    compiler_context
		.get_errors()
		.add_error(std::move(error));
	}
    }
    else if (operation.get_type() == Operation::OP_RETURN_VOID) {
	if (!return_type->is_void()) {
	    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Return statement returns incorrec type.");
	    error->add_message(
		operation.get_source_ref(),
		std::string("Return statement returns void (no value), but function is declared to return ") + return_type->get_name()
		);
	    error->add_message(
		function.get_source_ref(),
		"Return-type of function declared here."
		);
	    compiler_context
		.get_errors()
		.add_error(std::move(error));
	}
    }
}

void AnalysisPassReturnValues::check(const Function & function) const
{
    // This is what the function should return.
    ReturnOperationVisitor ppVisitor(get_compiler_context(), function);
    
    function.iterate_operations(ppVisitor);
}

