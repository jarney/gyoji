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

    // Ultimately, we will want to
    // define a relation on program points
    // so that:
    //      pp(x, a) < pp(x, a)
    //      pp(x, a) < pp(x, a+1)
    // for all a.  and pp(x,y) < pp(w,z)
    // if and only if w is reachable from
    // x through the control-flow graph.
    
    class ProgramPoint {
    public:
	ProgramPoint(size_t _block_id, size_t _operation_index);
	ProgramPoint(const ProgramPoint & other);
	~ProgramPoint();
	size_t block_id;
	size_t operation_index;
    };
    
    // This is an access of a local variable
    // inside a function.  Here, we track the
    // name of the variable, the tmpvar it's
    // loaded into, and the 'ProgramPoint' it was loaded/declared
    // from.
    class VariableLoadPoint {
    public:
	VariableLoadPoint(
	    std::string _variable_name,
	    ProgramPoint _program_point
	    );
	VariableLoadPoint(const VariableLoadPoint & other);
	~VariableLoadPoint();
	std::string variable_name;
	size_t tmpvar;
	ProgramPoint program_point;
    };

    class TmpvarAssignedAt {
    public:
	TmpvarAssignedAt(const TmpvarAssignedAt & other);
	TmpvarAssignedAt(
	    size_t _tmpvar,
	    ProgramPoint _program_point
	    );
	~TmpvarAssignedAt();
			 
	size_t tmpvar;
	ProgramPoint program_point;
    };

    class VariableTmpvarVisitor : public OperationVisitor {
    public:
	VariableTmpvarVisitor();
	~VariableTmpvarVisitor();
	void visit(
	    size_t block_id,
	    const BasicBlock & block,
	    size_t operation_index,
	    const Operation & operation
	    );
	const std::map<size_t, std::string> & get_tmpvars() const;
    private:
	std::map<size_t, std::string> tmpvars;
    };
        
    // First, we need to just build a map
    // of all the tmpvar to the variables they
    // represent (only for tmpvars associated with variables)
    class VariableUseVisitor : public OperationVisitor {
    public:
	VariableUseVisitor(
	    const std::map<size_t, std::string> & _tmpvars
	    );
	~VariableUseVisitor();
	void visit(
	    size_t block_id,
	    const BasicBlock & block,
	    size_t operation_index,
	    const Operation & operation
	    );
	const std::vector<VariableLoadPoint> & get_loads();
	const std::vector<VariableLoadPoint> & get_stores();
    private:
	const std::map<size_t, std::string> & tmpvars;
	std::vector<VariableLoadPoint> variable_loads;
	std::vector<VariableLoadPoint> variable_stores;
    };

    // Next, we need to find all of the places those tmpvars (variables)
    // are used and all of the places they are assigned.
    
};
//////////////////////////////////////////////
// ProgramPoint
//////////////////////////////////////////////

ProgramPoint::ProgramPoint(size_t _block_id, size_t _operation_index)
    : block_id(_block_id)
    , operation_index(_operation_index)
{}
ProgramPoint::ProgramPoint(const ProgramPoint & other)
    : block_id(other.block_id)
    , operation_index(other.operation_index)
{}
ProgramPoint::~ProgramPoint()
{}
//////////////////////////////////////////////
// VariableLoadPoint
//////////////////////////////////////////////
VariableLoadPoint::VariableLoadPoint(
    std::string _variable_name,
    ProgramPoint _program_point)
    : variable_name(_variable_name)
    , program_point(_program_point)
{}
VariableLoadPoint::VariableLoadPoint(const VariableLoadPoint & other)
    : variable_name(other.variable_name)
    , program_point(other.program_point)
{}

VariableLoadPoint::~VariableLoadPoint()
{}
			 
//////////////////////////////////////////////
// VariableUseVisitor
//////////////////////////////////////////////

VariableUseVisitor::VariableUseVisitor(
    const std::map<size_t, std::string> & _tmpvars
    )
    : OperationVisitor()
    , tmpvars(_tmpvars)
{
}

VariableUseVisitor::~VariableUseVisitor()
{}

const std::vector<VariableLoadPoint> &
VariableUseVisitor::get_loads()
{
    return variable_loads;
}

const std::vector<VariableLoadPoint> &
VariableUseVisitor::get_stores()
{ return variable_stores; }


void VariableUseVisitor::visit(
    size_t block_id,
    const BasicBlock & block,
    size_t operation_index,
    const Operation & operation
    )    
{
    const auto & operands = operation.get_operands();
    size_t noperands = operands.size();
    ProgramPoint program_point(block_id, operation_index);
    
    if (operation.get_type() == Operation::OP_ASSIGN) {
	const auto & it = tmpvars.find(operands.at(0));
	if (it != tmpvars.end()) {
	    VariableLoadPoint assigned_at(
		it->second,
		program_point
		);
	    variable_stores.push_back(assigned_at);
	}
	// All other operands are accesses.
	for (size_t operand_id = 1; operand_id < noperands; operand_id++) {
	    const auto & it = tmpvars.find(operands.at(operand_id));
	    if (it != tmpvars.end()) {
		VariableLoadPoint assigned_at(
		    it->second,
		    program_point
		    );
		variable_loads.push_back(assigned_at);
	    }
	}
    }
    else {
	// For other operations, all operands are 'use' of the variable.
	for (size_t operand_id = 0; operand_id < noperands; operand_id++) {
	    const auto & it = tmpvars.find(operands.at(operand_id));
	    if (it != tmpvars.end()) {
		VariableLoadPoint assigned_at(
		    it->second,
		    program_point
		    );
		variable_loads.push_back(assigned_at);
	    }
	}
    }
}
VariableTmpvarVisitor::VariableTmpvarVisitor()
{}

VariableTmpvarVisitor::~VariableTmpvarVisitor()
{}

void
VariableTmpvarVisitor::visit(
    size_t block_id,
    const BasicBlock & block,
    size_t operation_index,
    const Operation & operation
    )
{
    if (operation.get_type() == Operation::OP_LOCAL_VARIABLE) {
	const OperationLocalVariable &operation_local = (const OperationLocalVariable &)operation;
	// Just keep track of the tmpvars that map to local variables.
	tmpvars[operation_local.get_result()] = operation_local.get_symbol_name();
    }
}

const std::map<size_t, std::string> &
VariableTmpvarVisitor::get_tmpvars() const
{ return tmpvars; }

void AnalysisPassUseBeforeAssignment::check(const Function & function) const
{
    // TODO:
    // For each 'use' operation, we need to
    // check whether there exists a path 'backward' up the chain of
    // basic blocks where an assignment took place.
    VariableTmpvarVisitor id_visitor;
    function.iterate_operations(id_visitor);
    for (const auto & it : id_visitor.get_tmpvars()) {
	fprintf(stderr, "Tmpvar %ld %s\n", it.first, it.second.c_str());
    }
    
    VariableUseVisitor visitor(id_visitor.get_tmpvars());
    function.iterate_operations(visitor);
    
    fprintf(stderr, "Function %s\n", function.get_name().c_str());
    for (const auto & load : visitor.get_loads()) {
	fprintf(stderr, "Variable load var %s block %ld index %ld\n",
		load.variable_name.c_str(),
		load.program_point.block_id,
		load.program_point.operation_index);

	// Go back to all prior program points
	// and make sure it was initialized 'before'
	// this point.
//	if (stored_before(load.variable_name, load.program_point)) {
//	}
    }
    for (const auto & store : visitor.get_stores()) {
	fprintf(stderr, "tmpvar store %s block %ld index %ld\n",
		store.variable_name.c_str(),
		store.program_point.block_id,
		store.program_point.operation_index);
    }    

    // We now have all of the places
    // the variable was stored
    // and all of the places it was initialized.
    // Next for each load, we need to find out if
    
    // a) There exists a store for that variable
    // b) For all stores that exist, they all happen "before" the
    //    use.

    // If they're in the same BB, it's easy.
    // If they're not, we need to find all BB that are "before"
    // and ask the question on each.
    
}
